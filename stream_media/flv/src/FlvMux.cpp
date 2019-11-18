#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FlvMux.h"



namespace Flv {

class u4 
{
public:
	u4(unsigned int i) 
	{
		_u[0] = i >> 24;
		_u[1] = (i >> 16) & 0xff;
		_u[2] = (i >> 8) & 0xff;
		_u[3] = i & 0xff;
  	} 

public:
	unsigned char _u[4];
};

class u3 
{
public:
	u3(unsigned int i) 
	{
		_u[0] = i >> 16;
		_u[1] = (i >> 8) & 0xff;
		_u[2] = i & 0xff;
  	} 

public:
	unsigned char _u[3];
};

class u2 
{
public:
	u2(unsigned int i) {
		_u[0] = i >> 8;
		_u[1] = i & 0xff;
  	} 

public:
	unsigned char _u[2];
};


CFlvMux::CFlvMux()
: m_bWriteAACSeqHeader(false)
, m_bWriteAVCSeqHeader(false)
, m_pSPS(NULL)
, m_nSPSSize(0)
, m_pPPS(NULL)
, m_nPPSSize(0)
, m_nVideoTimeStamp(0)
{
}

CFlvMux::~CFlvMux()
{
}

IFlvMux *IFlvMux::Create()
{
	return new CFlvMux();
}

void CFlvMux::Destroy()
{
	delete this;
}

void CFlvMux::SetCallback(IFlvDataCallback *pcbk, void *context)
{
	m_context = context;
	m_pFlvCbk = pcbk;
}

bool CFlvMux::Start(bool bAudio, bool bVideo)
{
	u_char FlvHeader[FLV_HEADER_SIZE] = {0};

	FlvHeader[0] = 'F';
	FlvHeader[1] = 'L';
	FlvHeader[2] = 'V';
	FlvHeader[3] = 1;
	FlvHeader[4] = 0x0;

	if (bVideo != 0) {
		FlvHeader[4] |= 0x01;
	}

	if (bAudio != 0) {
		FlvHeader[4] |= 0x04;
	}

	unsigned int size = FLV_HEADER_SIZE;
	u4 size_u4(size);
	memcpy(FlvHeader + 5, size_u4._u, sizeof(unsigned int));
	
	if(m_pFlvCbk != NULL) {
		m_pFlvCbk->FlvPacket(m_context, FlvHeader, FLV_HEADER_SIZE, 0);
	}

	return true;
}

bool CFlvMux::Stop()
{
	if (m_pSPS != NULL) {
		delete m_pSPS;
		m_pSPS = NULL;
	}

	if (m_pPPS != NULL) {
		delete m_pPPS;
		m_pPPS = NULL;
	}

	if (m_nVideoTimeStamp != 0) {
		WriteH264EndofSeq();
	}
}

void CFlvMux::SetupTagHeader(int type, int datasize, uint32_t nTimeStamp, int streamid,
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

int CFlvMux::WriteAACHeader(u_char *pAAC, uint32_t nTimeStamp)
{
	TagHeader header;
	int nDataSize = 1 + 1 + 2;
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x08, nDataSize, nTimeStamp, 0, header, data);

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

	if(m_pFlvCbk != NULL) {
		m_pFlvCbk->FlvPacket(m_context, data, header.nDataSize + 11, header.nDataSize + 11 + 7);
	}

	m_bWriteAACSeqHeader = true;

	return 0;
}

void CFlvMux::WriteAACFrame(u_char *pFrame, int nFrameSize, uint32_t nTimeStamp)
{
	TagHeader header;
	int nDataSize = 1 + 1 + (nFrameSize - 7);
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x08, nDataSize, nTimeStamp, 0, header, data);

	u_char cAudioParam = 0xAF;
	memcpy(data + 11, &cAudioParam, 1);

	u_char cAACPacketType = 1;	/* AAC raw data */
	memcpy(data + 12, &cAACPacketType, 1);

	memcpy(data + 13, pFrame + 7, nFrameSize - 7);

	if(m_pFlvCbk != NULL) {
		m_pFlvCbk->FlvPacket(m_context, data, header.nDataSize + 11, header.nDataSize + 11 + 7);
	}

	return;
}

int CFlvMux::ConvertAAC(u_char *pAAC, int nAACFrameSize, uint32_t nTimeStamp)
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

void CFlvMux::WriteH264Header(uint32_t nTimeStamp)
{
	TagHeader header;
	int nDataSize = 1 + 1 + 3 + 6 + 2 + (m_nSPSSize - 4) + 1 + 2 + (m_nPPSSize - 4);
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x09, nDataSize, nTimeStamp, 0, header, data);

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

	if(m_pFlvCbk != NULL) {
		m_pFlvCbk->FlvPacket(m_context, data, header.nDataSize + 11, header.nDataSize + 11 + 7);
	}

	return;
}

void CFlvMux::WriteH264Frame(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp)
{
	TagHeader header;
	int nNaluType = pNalu[4] & 0x1f;
	int nDataSize = 1 + 1 + 3 + 4 + (nNaluSize - 4);
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x09, nDataSize, nTimeStamp, 0, header, data);

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

	if(m_pFlvCbk != NULL) {
		m_pFlvCbk->FlvPacket(m_context, data, header.nDataSize + 11, header.nDataSize + 11 + 7);
	}

	return;
}

void CFlvMux::WriteH264EndofSeq()
{
	TagHeader header;
	int nDataSize = 1 + 1 + 3;
	u_char *data = new u_char[nDataSize + 11];

	SetupTagHeader(0x09, nDataSize, m_nVideoTimeStamp, 0, header, data);

	data[11] = 0x27;
	data[12] = 0x02;

	u3 com_time_u3(0);
	memcpy(data + 13, com_time_u3._u, 3);

	if(m_pFlvCbk != NULL) {
		m_pFlvCbk->FlvPacket(m_context, data, header.nDataSize + 11, header.nDataSize + 11 + 7);
	}

	return;
}

int CFlvMux::ConvertH264(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp)
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

} // end namespace


