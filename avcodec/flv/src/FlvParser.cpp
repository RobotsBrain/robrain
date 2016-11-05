#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

	printf("TimeStamp: %d\n", header.nTotalTS);
	
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

void CFlvParser::GetTags(std::vector <CTag *> &vpTag)
{
	vpTag = m_vpTag;

	return;
}

void CFlvParser::GetFlvHeader(FlvHeader &flvHeader)
{
    memcpy(&flvHeader, m_pFlvHeader, sizeof(FlvHeader));

	return;
}


void CFlvParser::PrintFlvHeader()
{
	printf("FLV file version %u\n", m_pFlvHeader->nVersion);

    printf("  Contains audio tags: ");

    if (m_pFlvHeader->bHaveAudio) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }

    printf("  Contains video tags: ");

    if (m_pFlvHeader->bHaveVideo) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }

    printf("  Data offset: %lu\n", (unsigned long)m_pFlvHeader->nHeadSize);

    return;
}
