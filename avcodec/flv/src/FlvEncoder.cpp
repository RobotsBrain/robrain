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
	Stop();
}

bool CFlvEncoder::Start(bool bHaveAudio, bool bHaveVideo)
{
	FlvHeader Header;

	Header.nVersion = 0x01;
	Header.bHaveVideo = bHaveVideo;
	Header.bHaveAudio = bHaveAudio;
	Header.nHeadSize = 0x9;
	Header.pFlvHeader = new u_char[Header.nHeadSize];

	m_bHaveAudio = bHaveAudio;
	m_bHaveVideo = bHaveVideo;

	u_char *pFlvHeader = Header.pFlvHeader;

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

	CFlv::WriteHeaderData(Header, pFlvHeader);

	delete[] Header.pFlvHeader;

	return true;
}

bool CFlvEncoder::Stop()
{
	if (m_pSPS != NULL) {
		delete m_pSPS;
	}

	if (m_pPPS != NULL) {
		delete m_pPPS;
	}

	if (m_bHaveVideo != 0) {
		WriteH264EndofSeq();
	}

	return true;
}

void CFlvEncoder::SetupTagHeader(int type, int datasize, uint32_t nTimeStamp, int streamid,
								TagHeader &header, u_char *ptag)
{
	header.nType = type;
	header.nDataSize = datasize;
	header.nTimeStamp = nTimeStamp & 0x0fff;
	header.nTSEx = nTimeStamp >> 24;
	header.nStreamID = 0;
	header.nTotalTS = nTimeStamp;

	ptag[0] = type;

	u3 datasize_u3(datasize);
	memcpy(ptag + 1, datasize_u3._u, 3);

	u3 tt_u3(nTimeStamp);
	memcpy(ptag + 4, tt_u3._u, 3);

	ptag[7] = nTimeStamp >> 24;

	u3 sid_u3(streamid);
	memcpy(ptag + 8, sid_u3._u, 3);

	return;
}

int CFlvEncoder::WriteAACHeader(u_char *pAAC, uint32_t nTimeStamp)
{
	TagHeader header;
	int nDataSize = 1 + 1 + 2;
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x08, nDataSize, nTimeStamp, 0, header, data);
	// header.nType = 0x08;
	// header.nDataSize = datasize;
	// header.nTimeStamp = nTimeStamp;
	// header.nTSEx = 0;
	// header.nStreamID = 0;
	// header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp; // Fixme

	// data[0] = 0x08;

	// u3 datasize_u3(header.nDataSize);
	// memcpy(data + 1, datasize_u3._u, 3);

	// u3 tt_u3(nTimeStamp);
	// memcpy(data + 4, tt_u3._u, 3);

	// u_char cTTex = nTimeStamp >> 24;
	// memcpy(data + 7, &cTTex, 1);

	// u3 sid_u3(header.nStreamID);
	// memcpy(data + 8, sid_u3._u, 3);

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

	CFlv::WriteTag(pTag);

	delete[] data;

	m_bWriteAACSeqHeader = true;

	return 0;
}

void CFlvEncoder::WriteAACFrame(u_char *pFrame, int nFrameSize, uint32_t nTimeStamp)
{
	TagHeader header;
	int nDataSize = 1 + 1 + (nFrameSize - 7);
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x08, nDataSize, nTimeStamp, 0, header, data);

	// header.nType = 0x08;
	// header.nDataSize = 1 + 1 + (nFrameSize - 7);
	// header.nTimeStamp = nTimeStamp;
	// header.nTSEx = 0;
	// header.nStreamID = 0;
	// header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp; // Fixme

	// u_char *data = new u_char[header.nDataSize + 11];

	// data[0] = 0x08;

	// u3 datasize_u3(header.nDataSize);
	// memcpy(data + 1, datasize_u3._u, 3);

	// u3 tt_u3(nTimeStamp);
	// memcpy(data + 4, tt_u3._u, 3);

	// u_char cTTex = nTimeStamp >> 24;
	// memcpy(data + 7, &cTTex, 1);

	// u3 sid_u3(header.nStreamID);
	// memcpy(data + 8, sid_u3._u, 3);

	u_char cAudioParam = 0xAF;
	memcpy(data + 11, &cAudioParam, 1);

	u_char cAACPacketType = 1;	/* AAC raw data */
	memcpy(data + 12, &cAACPacketType, 1);

	memcpy(data + 13, pFrame + 7, nFrameSize - 7);

	CTag *pTag = NULL;

	pTag = new CAudioTag(&header, data, header.nDataSize + 11, this);

	CFlv::WriteTag(pTag);

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

void CFlvEncoder::WriteH264Header(uint32_t nTimeStamp)
{
	TagHeader header;
	int nDataSize = 1 + 1 + 3 + 6 + 2 + (m_nSPSSize - 4) + 1 + 2 + (m_nPPSSize - 4);
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x09, nDataSize, nTimeStamp, 0, header, data);

	// 

	// header.nType = 0x09;
	// header.nDataSize = nDataSize;
	// header.nTimeStamp = nTimeStamp;
	// header.nTSEx = 0;
	// header.nStreamID = 0;
	// header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp;

	// data[0] = 0x09;

	// u3 datasize_u3(nDataSize);
	// memcpy(data + 1, datasize_u3._u, 3);

	// u3 tt_u3(nTimeStamp);
	// memcpy(data + 4, tt_u3._u, 3);

	// unsigned char cTTex = nTimeStamp >> 24;
	// memcpy(data + 7, &cTTex, 1);

	// u3 sid_u3(0);
	// memcpy(data + 8, sid_u3._u, 3);

	u_char cVideoParam = 0x17;
	memcpy(data + 11, &cVideoParam, 1);

	u_char cAVCPacketType = 0;	/* seq header */
	memcpy(data + 12, &cAVCPacketType, 1);

	u3 CompositionTime_u3(0);
	memcpy(data + 13, CompositionTime_u3._u, 3);

	data[16] = 1;
	data[17] = m_pSPS[5];
	data[18] = m_pSPS[6];
	data[19] = m_pSPS[7];
	data[20] = 0xff;
	data[21] = 0xE1;

	u2 spssize_u2(m_nSPSSize - 4);
	memcpy(data + 22, spssize_u2._u, 2);

	memcpy(data + 24, (m_pSPS + 4), m_nSPSSize - 4);

	data[24 + m_nSPSSize - 4] = 0x01;

	u2 ppssize_u2(m_nPPSSize - 4);
	memcpy(data + 24 + m_nSPSSize - 4 + 1, ppssize_u2._u, 2);
	memcpy(data + 24 + m_nSPSSize - 4 + 1 + 2, (m_pPPS + 4), m_nPPSSize - 4);

	CTag *pTag = NULL;

	pTag = new CAudioTag(&header, data, header.nDataSize + 11, this);

	CFlv::WriteTag(pTag);

	delete[] data;

	return;
}

void CFlvEncoder::WriteH264Frame(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp)
{
	int nNaluType = pNalu[4] & 0x1f;
	int nDataSize = 1 + 1 + 3 + 4 + (nNaluSize - 4);

	u_char *data = new u_char[nDataSize + 11];

	TagHeader header;

	SetupTagHeader(0x09, nDataSize, nTimeStamp, 0, header, data);

	// header.nType = 0x09;
	// header.nDataSize = nDataSize;
	// header.nTimeStamp = nTimeStamp;
	// header.nTSEx = 0;
	// header.nStreamID = 0;
	// header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp;

	// data[0] = 0x09;

	// u3 datasize_u3(nDataSize);
	// memcpy(data + 1, datasize_u3._u, 3);

	// u3 tt_u3(nTimeStamp);
	// memcpy(data + 4, tt_u3._u, 3);

	// unsigned char cTTex = nTimeStamp >> 24;
	// memcpy(data + 7, &cTTex, 1);

	// u3 sid_u3(0);
	// memcpy(data + 8, sid_u3._u, 3);

	if (nNaluType == 5) {
		data[11] = 0x17;
	} else {
		data[11] = 0x27;
	}

	data[12] = 1;

	u3 com_time_u3(0);
	memcpy(data + 13, com_time_u3._u, 3);

	u4 nalusize_u4(nNaluSize - 4);
	memcpy(data + 16, nalusize_u4._u, 4);

	memcpy(data + 20, pNalu + 4, nNaluSize - 4);

	CTag *pTag = NULL;

	pTag = new CAudioTag(&header, data, header.nDataSize + 11, this);

	CFlv::WriteTag(pTag);

	delete[] data;

	return;
}

void CFlvEncoder::WriteH264EndofSeq()
{
	int nDataSize = 1 + 1 + 3;

	u_char *data = new u_char[nDataSize + 11];

	TagHeader header;

	SetupTagHeader(0x09, nDataSize, m_nVideoTimeStamp, 0, header, data);

	// header.nType = 0x09;
	// header.nDataSize = nDataSize;
	// header.nTimeStamp = m_nVideoTimeStamp;
	// header.nTSEx = 0;
	// header.nStreamID = 0;
	// header.nTotalTS = (uint32_t)((header.nTSEx << 24)) + header.nTimeStamp;

	// data[0] = 0x09;

	// u3 datasize_u3(nDataSize);
	// memcpy(data + 1, datasize_u3._u, 3);

	// u3 tt_u3(m_nVideoTimeStamp);
	// memcpy(data + 4, tt_u3._u, 3);

	// unsigned char cTTex = m_nVideoTimeStamp >> 24;
	// memcpy(data + 7, &cTTex, 1);

	// u3 sid_u3(0);
	// memcpy(data + 8, sid_u3._u, 3);

	data[11] = 0x27;
	data[12] = 0x02;

	u3 com_time_u3(0);
	memcpy(data + 13, com_time_u3._u, 3);

	CTag *pTag = NULL;

	pTag = new CAudioTag(&header, data, header.nDataSize + 11, this);

	CFlv::WriteTag(pTag);

	delete[] data;

	return;
}

int CFlvEncoder::ConvertH264(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp)
{
	m_nVideoTimeStamp = nTimeStamp;

	if (pNalu == NULL || nNaluSize <= 4) {
		return -1;
	}

	int nNaluType = pNalu[4] & 0x1f;

	if (m_pSPS == NULL && nNaluType == 0x07) {
		m_pSPS = new u_char[nNaluSize];
		m_nSPSSize = nNaluSize;
		memcpy(m_pSPS, pNalu, nNaluSize);
	}

	if (m_pPPS == NULL && nNaluType == 0x08) {
		m_pPPS = new u_char[nNaluSize];
		m_nPPSSize = nNaluSize;
		memcpy(m_pPPS, pNalu, nNaluSize);
	}

	if (m_pSPS != NULL && m_pPPS != NULL && !m_bWriteAVCSeqHeader) {
		WriteH264Header(nTimeStamp);
		m_bWriteAVCSeqHeader = true;
	}

	if (!m_bWriteAVCSeqHeader) {
		return -1;
	}

	WriteH264Frame(pNalu, nNaluSize, nTimeStamp);

	return 0;
}


