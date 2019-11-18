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
: m_bFlvHeader(false)
{
}

CFlvParser::~CFlvParser()
{
}

int CFlvParser::Parse(u_char *pBuf, int nBufSize, int &nUsedLen)
{
	int nOffset = 0;

	if (!m_bFlvHeader) {
		CheckBuffer(9);

		FlvHeader Header;
		u_char *p = pBuf + nOffset;

		Header.nVersion = p[3];
		Header.bHaveAudio = (p[4] >> 2) & 0x01;
		Header.bHaveVideo = (p[4] >> 0) & 0x01;
		Header.nHeadSize = ShowU32(p + 5);

		CFlv::WriteHeaderData(Header, pBuf);

		nOffset += Header.nHeadSize;
		m_bFlvHeader = true;
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

		CFlv::WriteTag(pTag);
	}

	nUsedLen = nOffset;

	return 0;
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
		pTag->Init(&header, pBuf, nLeftLen);
	}

	return pTag;
}


