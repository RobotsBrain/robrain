#include <fstream>

#include "Common.h"
#include "AudioTag.h"
#include "VideoTag.h"
#include "FlvDumpFile.h"

using namespace std;



void PrintInfo(CFlvParser *pFlvParser)
{
	vector <CTag *> vpTag;

	pFlvParser->GetTags(vpTag);

	pFlvParser->PrintFlvHeader();

	vector <CTag *>::iterator it_tag;

	for (it_tag = vpTag.begin(); it_tag < vpTag.end(); it_tag++) {

		(*it_tag)->PrintTagHeader();

		int type = (*it_tag)->GetType();

		if(type == 0x08) {
			((CAudioTag *)(*it_tag))->PrintAudioTag();
		} else if (type == 0x09) {
			((CVideoTag *)(*it_tag))->PrintVideoTag();
		} else if (type == 0x12) {

		}
	}

	return;
}

int DumpH264(CFlvParser *pFlvParser, const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	vector <CTag *> vpTag;

	pFlvParser->GetTags(vpTag);

	vector <CTag *>::iterator it_tag;

	for (it_tag = vpTag.begin(); it_tag != vpTag.end(); it_tag++) {
		if ((*it_tag)->GetType() != 0x09) {
			continue;
		}

		f.write((char *)(*it_tag)->GetMedia(), (*it_tag)->GetMediaLen());
	}

	f.close();

	return 0;
}

int DumpAAC(CFlvParser *pFlvParser, const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	vector <CTag *> vpTag;

	pFlvParser->GetTags(vpTag);

	vector <CTag *>::iterator it_tag;

	for (it_tag = vpTag.begin(); it_tag != vpTag.end(); it_tag++) {
		if ((*it_tag)->GetType() != 0x08)
			continue;

		CAudioTag *pAudioTag = (CAudioTag *)(*it_tag);
		// if (pAudioTag->_nSoundFormat != 10)
		// 	continue;

		if (pAudioTag->GetMediaLen() != 0)
			f.write((char *)(*it_tag)->GetMedia(), (*it_tag)->GetMediaLen());
	}

	f.close();

	return 0;
}

int DumpFlv(CFlvParser *pFlvParser, const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	vector <CTag *> vpTag;
	int nNalUnitLength = 0;
	FlvHeader flvHeader;

	pFlvParser->GetTags(vpTag);
	pFlvParser->GetNalUnitLength(nNalUnitLength);
	pFlvParser->GetFlvHeader(flvHeader);

	// write flv-header
	f.write((char *)flvHeader.pFlvHeader, flvHeader.nHeadSize);
	unsigned int nLastTagSize = 0;

	// write flv-tag
	vector <CTag *>::iterator it_tag;

	for (it_tag = vpTag.begin(); it_tag < vpTag.end(); it_tag++) {
		unsigned int nn = WriteU32(nLastTagSize);
		f.write((char *)&nn, 4);

		u_char *pTagHeader = (*it_tag)->GetTagHeaderData();
		u_char *pTagData = (*it_tag)->GetTagData();
		uint32_t datasize = (*it_tag)->GetDataSize();

		//check duplicate start code
		if ((*it_tag)->GetType() == 0x09 && *(pTagData + 1) == 0x01) {
			bool duplicate = false;
			u_char *pStartCode = pTagData + 5 + nNalUnitLength;

			unsigned nalu_len = 0;
			u_char *p_nalu_len = (u_char *)&nalu_len;

			switch (nNalUnitLength) {
			case 4:
				nalu_len = ShowU32(pTagData + 5);
				break;

			case 3:
				nalu_len = ShowU24(pTagData + 5);
				break;

			case 2:
				nalu_len = ShowU16(pTagData + 5);
				break;

			default:
				nalu_len = ShowU8(pTagData + 5);
				break;
			}

			u_char *pStartCodeRecord = pStartCode;
			int i;

			for (i = 0; i < datasize - 5 - nNalUnitLength - 4; ++i) {
				if (pStartCode[i] == 0x00 && pStartCode[i + 1] == 0x00
					&& pStartCode[i + 2] == 0x00 && pStartCode[i + 3] == 0x01) {

					if (pStartCode[i + 4] == 0x67) {
						//printf("duplicate sps found!\n");
						i += 4;
						continue;
					} else if (pStartCode[i + 4] == 0x68) {
						//printf("duplicate pps found!\n");
						i += 4;
						continue;
					} else if (pStartCode[i + 4] == 0x06) {
						//printf("duplicate sei found!\n");
						i += 4;
						continue;
					} else {
						i += 4;
						//printf("offset=%d\n",i);
						duplicate = true;
						break;
					}
				}
			}

			if (duplicate) {
				nalu_len -= i;
				datasize -= i;
				u_char *p = (u_char *)&(datasize);
				pTagHeader[1] = p[2];
				pTagHeader[2] = p[1];
				pTagHeader[3] = p[0];

				f.write((char *)pTagHeader, 11);

				switch (nNalUnitLength) {
				case 4:
					*(pTagData + 5) = p_nalu_len[3];
					*(pTagData + 6) = p_nalu_len[2];
					*(pTagData + 7) = p_nalu_len[1];
					*(pTagData + 8) = p_nalu_len[0];
					break;

				case 3:
					*(pTagData + 5) = p_nalu_len[2];
					*(pTagData + 6) = p_nalu_len[1];
					*(pTagData + 7) = p_nalu_len[0];
					break;

				case 2:
					*(pTagData + 5) = p_nalu_len[1];
					*(pTagData + 6) = p_nalu_len[0];
					break;

				default:
					*(pTagData + 5) = p_nalu_len[0];
					break;
				}

				f.write((char *)pTagData, pStartCode - pTagData);
				f.write((char *)pStartCode + i, datasize - (pStartCode - pTagData));
			} else {
				f.write((char *)pTagHeader, 11);
				f.write((char *)pTagData, datasize);
			}
		} else {
			f.write((char *)pTagHeader, 11);
			f.write((char *)pTagData, datasize);
		}

		nLastTagSize = 11 + datasize;
	}

	unsigned int nn = WriteU32(nLastTagSize);
	f.write((char *)&nn, 4);

	f.close();

	return 0;
}


