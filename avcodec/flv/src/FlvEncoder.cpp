#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "AudioTag.h"
#include "VideoTag.h"
#include "FlvEncoder.h"



CFlvEncoder::CFlvEncoder()
{
}

CFlvEncoder::~CFlvEncoder()
{
}

bool CFlvEncoder::Start(bool bHaveAudio, bool bHaveVideo)
{
	m_bHaveAudio = bHaveAudio;
	m_bHaveVideo = bHaveVideo;

	m_FlvHeader.nVersion = 0x01;
	m_FlvHeader.bHaveVideo = bHaveVideo;
	m_FlvHeader.bHaveAudio = bHaveAudio;
	m_FlvHeader.nHeadSize = 0x9;
	m_FlvHeader.pFlvHeader = new u_char[m_FlvHeader.nHeadSize];

	u_char *pFlvHeader = m_FlvHeader.pFlvHeader;

	pFlvHeader[0] = 'F';
	pFlvHeader[1] = 'L';
	pFlvHeader[2] = 'V';
	pFlvHeader[3] = 1;
	pFlvHeader[4] = 0x0;

	if (bHaveVideo != 0) {
		pFlvHeader[4] |= 0x01;
	}

	if (bHaveAudio != 0) {
		pFlvHeader[4] |= 0x04;
	}

	unsigned int size = 9;
	u4 size_u4(size);
	memcpy(pFlvHeader + 5, size_u4._u, sizeof(unsigned int));

	return true;
}

bool CFlvEncoder::Stop()
{
	return true;
}

int CFlvEncoder::WriteAACHeader(u_char *pAAC, uint32_t nTimeStamp)
{
	TagHeader header;

	header.nType = 0x08;
	header.nDataSize = 1 + 1 + 2;
	header.nTimeStamp = nTimeStamp;
	header.nTSEx = 0;
	header.nStreamID = 0;
	header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp; // Fixme

	u_char *data = new u_char[header.nDataSize + 11];

	data[0] = 0x08;

	u3 datasize_u3(header.nDataSize);
	memcpy(data + 1, datasize_u3._u, 3);

	u3 tt_u3(nTimeStamp);
	memcpy(data + 4, tt_u3._u, 3);

	u_char cTTex = nTimeStamp >> 24;
	memcpy(data + 7, &cTTex, 1);

	u3 sid_u3(header.nStreamID);
	memcpy(data + 8, sid_u3._u, 3);

	u_char cAudioParam = 0xAF;
	memcpy(data + 11, &cAudioParam, 1);

	u_char cAACPacketType = 0;	/* seq header */
	memcpy(data + 12, &cAACPacketType, 1);

	u_char AudioSpecificConfig[2];
	u_char *p = (u_char *)pAAC;

	char aacProfile = (p[2] >> 6) + 1;
	char sampleRateIndex = (p[2] >> 2) & 0x0f;
	char channelConfig = ((p[2] & 0x01) << 2) + (p[3] >> 6);

	AudioSpecificConfig[0] = (aacProfile << 3) + (sampleRateIndex >> 1);
	AudioSpecificConfig[1] = ((sampleRateIndex & 0x01) << 7) + (channelConfig << 3);
	
	memcpy(data + 13, (char *)AudioSpecificConfig, 2);

	CTag *pTag = NULL;

	pTag = new CAudioTag(&header, data, header.nDataSize + 11, this);

	m_vpTag.push_back(pTag);

	delete[] data;

	m_bWriteAACSeqHeader = true;

	return 0;
}

void CFlvEncoder::WriteAACFrame(u_char *pFrame, int nFrameSize, uint32_t nTimeStamp)
{
	TagHeader header;

	header.nType = 0x08;
	header.nDataSize = 1 + 1 + (nFrameSize - 7);
	header.nTimeStamp = nTimeStamp;
	header.nTSEx = 0;
	header.nStreamID = 0;
	header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp; // Fixme

	u_char *data = new u_char[header.nDataSize + 11];

	data[0] = 0x08;

	u3 datasize_u3(header.nDataSize);
	memcpy(data + 1, datasize_u3._u, 3);

	u3 tt_u3(nTimeStamp);
	memcpy(data + 4, tt_u3._u, 3);

	u_char cTTex = nTimeStamp >> 24;
	memcpy(data + 7, &cTTex, 1);

	u3 sid_u3(header.nStreamID);
	memcpy(data + 8, sid_u3._u, 3);

	u_char cAudioParam = 0xAF;
	memcpy(data + 11, &cAudioParam, 1);

	u_char cAACPacketType = 1;	/* AAC raw data */
	memcpy(data + 12, &cAACPacketType, 1);

	memcpy(data + 13, pFrame + 7, nFrameSize - 7);

	CTag *pTag = NULL;

	pTag = new CAudioTag(&header, data, header.nDataSize + 11, this);

	m_vpTag.push_back(pTag);

	delete[] data;

	return;
}

int CFlvEncoder::ConvertAAC(u_char *pAAC, int nAACFrameSize, uint32_t nTimeStamp)
{
	if (pAAC == NULL || nAACFrameSize <= 7) {
		return -1;
	}

	if(!m_bWriteAACSeqHeader) {
		WriteAACHeader(pAAC, nTimeStamp);
	} else if(m_bWriteAACSeqHeader) {
		WriteAACFrame(pAAC, nAACFrameSize, nTimeStamp);
	}

	return 0;
}


