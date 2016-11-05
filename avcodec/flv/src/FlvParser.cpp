#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <fstream>

#include "Common.h"
#include "AudioTag.h"
#include "VideoTag.h"
#include "FlvParser.h"

using namespace std;

#define CheckBuffer(x) { if ((nBufSize-nOffset)<(x)) { nUsedLen = nOffset; return 0;} }



CFlvParser::CFlvParser()
: m_pFlvHeader(NULL)
{
}

CFlvParser::~CFlvParser()
{
	DestroyTag();
	DestroyFlvHeader();
}

int CFlvParser::Parse(u_char *pBuf, int nBufSize, int &nUsedLen)
{
	int nOffset = 0;

	if (m_pFlvHeader == NULL) {
		CheckBuffer(9);
		m_pFlvHeader = CreateFlvHeader(pBuf + nOffset);
		nOffset += m_pFlvHeader->nHeadSize;
	}

	while (1) {
		CheckBuffer(15);

		int nPrevSize = ShowU32(pBuf + nOffset);

		nOffset += 4;

		CTag *pTag = CreateTag(pBuf + nOffset, nBufSize - nOffset);
		if (pTag == NULL) {
			nOffset -= 4;
			break;
		}

		nOffset += (11 + pTag->GetDataSize());

		m_vpTag.push_back(pTag);
	}

	nUsedLen = nOffset;

	return 0;
}

FlvHeader *CFlvParser::CreateFlvHeader(u_char *pBuf)
{
	FlvHeader *pHeader = new FlvHeader;

	pHeader->nVersion = pBuf[3];
	pHeader->bHaveAudio = (pBuf[4] >> 2) & 0x01;
	pHeader->bHaveVideo = (pBuf[4] >> 0) & 0x01;
	pHeader->nHeadSize = ShowU32(pBuf + 5);

	pHeader->pFlvHeader = new u_char[pHeader->nHeadSize];
	memcpy(pHeader->pFlvHeader, pBuf, pHeader->nHeadSize);

	return pHeader;
}

void CFlvParser::DestroyFlvHeader()
{
	if (m_pFlvHeader != NULL) {
		delete m_pFlvHeader->pFlvHeader;
		delete m_pFlvHeader;
	}

	return;
}

CTag *CFlvParser::CreateTag(u_char *pBuf, int nLeftLen)
{
	TagHeader header;

	header.nType = ShowU8(pBuf + 0);
	header.nDataSize = ShowU24(pBuf + 1);
	header.nTimeStamp = ShowU24(pBuf + 4);
	header.nTSEx = ShowU8(pBuf + 7);
	header.nStreamID = ShowU24(pBuf + 8);
	header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp;

	cout << "TimeStamp : " << header.nTotalTS << endl;
	//cout << "nLeftLen : " << nLeftLen << " , nDataSize : " << pTag->header.nDataSize << endl;
	
	if((header.nDataSize + 11) > nLeftLen) {
		return NULL;
	}

	CTag *pTag = NULL;

	switch (header.nType) {
	case 0x09:
		pTag = new CVideoTag(&header, pBuf, nLeftLen, this);
		break;

	case 0x08:
		pTag = new CAudioTag(&header, pBuf, nLeftLen, this);
		break;

	default:
		pTag = new CTag();
		pTag->Init(&header, pBuf, nLeftLen, this);
	}

	return pTag;
}

void CFlvParser::DestroyTag()
{
	for (int i = 0; i < m_vpTag.size(); i++) {
		delete m_vpTag[i];
	}

	return;
}

void CFlvParser::SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig)
{
	m_aacProfile = aacProfile;
	m_sampleRateIndex = sampleRateIndex;
	m_channelConfig = channelConfig;
}

void CFlvParser::GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig)
{
	aacProfile = m_aacProfile;
	sampleRateIndex = m_sampleRateIndex;
	channelConfig = m_channelConfig;
}

void CFlvParser::SetNalUnitLength(int len)
{
	m_nNalUnitLength = len;
}

void CFlvParser::GetNalUnitLength(int &len)
{
	len = m_nNalUnitLength;
}

/***********************************************************************************************/


int CFlvParser::DumpH264(const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	vector <CTag *>::iterator it_tag;

	for (it_tag = m_vpTag.begin(); it_tag != m_vpTag.end(); it_tag++) {
		if ((*it_tag)->GetType() != 0x09) {
			continue;
		}

		f.write((char *)(*it_tag)->GetMedia(), (*it_tag)->GetMediaLen());
	}

	f.close();

	return 1;
}

int CFlvParser::DumpAAC(const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	vector <CTag *>::iterator it_tag;

	for (it_tag = m_vpTag.begin(); it_tag != m_vpTag.end(); it_tag++) {
		if ((*it_tag)->GetType() != 0x08)
			continue;

		CAudioTag *pAudioTag = (CAudioTag *)(*it_tag);
		// if (pAudioTag->_nSoundFormat != 10)
		// 	continue;

		if (pAudioTag->GetMediaLen() != 0)
			f.write((char *)(*it_tag)->GetMedia(), (*it_tag)->GetMediaLen());
	}

	f.close();

	return 1;
}

#if 0
int CFlvParser::DumpFlv(const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	// write flv-header
	f.write((char *)m_pFlvHeader->pFlvHeader, m_pFlvHeader->nHeadSize);
	unsigned int nLastTagSize = 0;

	// write flv-tag
	vector < Tag * >::iterator it_tag;
	for (it_tag = m_vpTag.begin(); it_tag < m_vpTag.end(); it_tag++) {
		unsigned int nn = WriteU32(nLastTagSize);
		f.write((char *)&nn, 4);

		//check duplicate start code
		if ((*it_tag)->_header.nType == 0x09
			&& *((*it_tag)->_pTagData + 1) == 0x01) {
			bool duplicate = false;
			u_char *pStartCode = (*it_tag)->_pTagData + 5 + m_nNalUnitLength;
			//printf("tagsize=%d\n",(*it_tag)->_header.nDataSize);
			unsigned nalu_len = 0;
			u_char *p_nalu_len = (u_char *)&nalu_len;
			switch (m_nNalUnitLength) {
			case 4:
				nalu_len = ShowU32((*it_tag)->_pTagData + 5);
				break;

			case 3:
				nalu_len = ShowU24((*it_tag)->_pTagData + 5);
				break;

			case 2:
				nalu_len = ShowU16((*it_tag)->_pTagData + 5);
				break;

			default:
				nalu_len = ShowU8((*it_tag)->_pTagData + 5);
				break;
			}
			/*
			   printf("nalu_len=%u\n",nalu_len);
			   printf("%x,%x,%x,%x,%x,%x,%x,%x,%x\n",(*it_tag)->_pTagData[5],(*it_tag)->_pTagData[6],
			   (*it_tag)->_pTagData[7],(*it_tag)->_pTagData[8],(*it_tag)->_pTagData[9],
			   (*it_tag)->_pTagData[10],(*it_tag)->_pTagData[11],(*it_tag)->_pTagData[12],
			   (*it_tag)->_pTagData[13]);
			 */

			u_char *pStartCodeRecord = pStartCode;
			int i;

			for (i = 0;
				 i < (*it_tag)->_header.nDataSize - 5 - m_nNalUnitLength - 4;
				 ++i) {
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
				(*it_tag)->_header.nDataSize -= i;
				u_char *p =
					(u_char *)&((*it_tag)->_header.nDataSize);
				(*it_tag)->_pTagHeader[1] = p[2];
				(*it_tag)->_pTagHeader[2] = p[1];
				(*it_tag)->_pTagHeader[3] = p[0];
				//printf("after,tagsize=%d\n",(int)ShowU24((*it_tag)->_pTagHeader + 1));
				//printf("%x,%x,%x\n",(*it_tag)->_pTagHeader[1],(*it_tag)->_pTagHeader[2],(*it_tag)->_pTagHeader[3]);

				f.write((char *)(*it_tag)->_pTagHeader, 11);
				switch (m_nNalUnitLength) {
				case 4:
					*((*it_tag)->_pTagData + 5) = p_nalu_len[3];
					*((*it_tag)->_pTagData + 6) = p_nalu_len[2];
					*((*it_tag)->_pTagData + 7) = p_nalu_len[1];
					*((*it_tag)->_pTagData + 8) = p_nalu_len[0];
					break;

				case 3:
					*((*it_tag)->_pTagData + 5) = p_nalu_len[2];
					*((*it_tag)->_pTagData + 6) = p_nalu_len[1];
					*((*it_tag)->_pTagData + 7) = p_nalu_len[0];
					break;

				case 2:
					*((*it_tag)->_pTagData + 5) = p_nalu_len[1];
					*((*it_tag)->_pTagData + 6) = p_nalu_len[0];
					break;

				default:
					*((*it_tag)->_pTagData + 5) = p_nalu_len[0];
					break;
				}
				//printf("after,nalu_len=%d\n",(int)ShowU32((*it_tag)->_pTagData + 5));
				f.write((char *)(*it_tag)->_pTagData, pStartCode - (*it_tag)->_pTagData);
				/*
				   printf("%x,%x,%x,%x,%x,%x,%x,%x,%x\n",(*it_tag)->_pTagData[0],(*it_tag)->_pTagData[1],(*it_tag)->_pTagData[2],
				   (*it_tag)->_pTagData[3],(*it_tag)->_pTagData[4],(*it_tag)->_pTagData[5],(*it_tag)->_pTagData[6],
				   (*it_tag)->_pTagData[7],(*it_tag)->_pTagData[8]);
				 */
				f.write((char *)pStartCode + i,
						(*it_tag)->_header.nDataSize - (pStartCode -
														(*it_tag)->_pTagData));
				/*
				   printf("write size:%d\n", (pStartCode - (*it_tag)->_pTagData) +
				   ((*it_tag)->_header.nDataSize - (pStartCode - (*it_tag)->_pTagData)));
				 */
			} else {
				f.write((char *)(*it_tag)->_pTagHeader, 11);
				f.write((char *)(*it_tag)->_pTagData, (*it_tag)->_header.nDataSize);
			}
		} else {
			f.write((char *)(*it_tag)->_pTagHeader, 11);
			f.write((char *)(*it_tag)->_pTagData, (*it_tag)->_header.nDataSize);
		}

		nLastTagSize = 11 + (*it_tag)->_header.nDataSize;
	}

	unsigned int nn = WriteU32(nLastTagSize);
	f.write((char *)&nn, 4);

	f.close();

	return 1;
}

#endif
