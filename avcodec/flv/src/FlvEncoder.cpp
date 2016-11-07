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
	FlvHeader Header;

	Header.nVersion = 0x01;
	Header.bHaveVideo = bHaveVideo;
	Header.bHaveAudio = bHaveAudio;
	Header.nHeadSize = 0x9;
	Header.pFlvHeader = new u_char[Header.nHeadSize];

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

	CFlv::WriteTag(pTag);

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

#if 0
void CFlvEncoder::WriteH264Header(unsigned int nTimeStamp)
{
	u4 prev_u4(_nPrevTagSize);
	_fileOut.write((char *)prev_u4._u, 4);

	char cTagType = 0x09;
	_fileOut.write(&cTagType, 1);
	int nDataSize = 1 + 1 + 3 + 6 + 2 + (_nSPSSize - 4) + 1 + 2 + (_nPPSSize - 4);

	u3 datasize_u3(nDataSize);
	_fileOut.write((char *)datasize_u3._u, 3);

	u3 tt_u3(nTimeStamp);
	_fileOut.write((char *)tt_u3._u, 3);

	unsigned char cTTex = nTimeStamp >> 24;
	_fileOut.write((char *)&cTTex, 1);

	u3 sid_u3(_nStreamID);
	_fileOut.write((char *)sid_u3._u, 3);

	unsigned char cVideoParam = 0x17;
	_fileOut.write((char *)&cVideoParam, 1);
	unsigned char cAVCPacketType = 0;	/* seq header */
	_fileOut.write((char *)&cAVCPacketType, 1);

	u3 CompositionTime_u3(0);
	_fileOut.write((char *)CompositionTime_u3._u, 3);

	Write(1);
	Write(_pSPS[5]);
	Write(_pSPS[6]);
	Write(_pSPS[7]);
	Write((unsigned char)(0xff));
	Write((unsigned char)(0xE1));

	u2 spssize_u2(_nSPSSize - 4);
	_fileOut.write((char *)spssize_u2._u, 2);
	_fileOut.write((char *)(_pSPS + 4), _nSPSSize - 4);
	Write((unsigned char)(0x01));

	u2 ppssize_u2(_nPPSSize - 4);
	_fileOut.write((char *)ppssize_u2._u, 2);
	_fileOut.write((char *)(_pPPS + 4), _nPPSSize - 4);

	_nPrevTagSize = 11 + nDataSize;
}

void CFlvEncoder::WriteH264Frame(char *pNalu, int nNaluSize, unsigned int nTimeStamp)
{
	int nNaluType = pNalu[4] & 0x1f;
	if (nNaluType == 7 || nNaluType == 8) {
		return;
	}

	u4 prev_u4(_nPrevTagSize);
	Write(prev_u4);

	Write(0x09);
	int nDataSize;
	nDataSize = 1 + 1 + 3 + 4 + (nNaluSize - 4);
	u3 datasize_u3(nDataSize);
	Write(datasize_u3);
	u3 tt_u3(nTimeStamp);
	Write(tt_u3);
	Write((unsigned char)(nTimeStamp >> 24));

	u3 sid(_nStreamID);
	Write(sid);

	if (nNaluType == 5) {
		Write(0x17);
	} else {
		Write(0x27);
	}

	Write((unsigned char)(1));
	u3 com_time_u3(0);
	Write(com_time_u3);

	u4 nalusize_u4(nNaluSize - 4);
	Write(nalusize_u4);

	_fileOut.write((char *)(pNalu + 4), nNaluSize - 4);

	_nPrevTagSize = 11 + nDataSize;
}

void CFlvEncoder::WriteH264EndofSeq()
{
	u4 prev_u4(_nPrevTagSize);
	Write(prev_u4);

	Write(0x09);
	int nDataSize;
	nDataSize = 1 + 1 + 3;
	u3 datasize_u3(nDataSize);
	Write(datasize_u3);
	u3 tt_u3(_nVideoTimeStamp);
	Write(tt_u3);
	Write((unsigned char)(_nVideoTimeStamp >> 24));

	u3 sid(_nStreamID);
	Write(sid);

	Write(0x27);
	Write(0x02);

	u3 com_time_u3(0);
	Write(com_time_u3);
}

int CFlvEncoder::ConvertH264(char *pNalu, int nNaluSize, unsigned int nTimeStamp)
{
	_nVideoTimeStamp = nTimeStamp;

	if (pNalu == NULL || nNaluSize <= 4) {
		return 0;
	}

	int nNaluType = pNalu[4] & 0x1f;
	if (_pSPS == NULL && nNaluType == 0x07) {
		_pSPS = new unsigned char[nNaluSize];
		_nSPSSize = nNaluSize;
		memcpy(_pSPS, pNalu, nNaluSize);
	}

	if (_pPPS == NULL && nNaluType == 0x08) {
		_pPPS = new unsigned char[nNaluSize];
		_nPPSSize = nNaluSize;
		memcpy(_pPPS, pNalu, nNaluSize);
	}

	if (_pSPS != NULL && _pPPS != NULL && _bWriteAVCSeqHeader == 0) {
		WriteH264Header(nTimeStamp);
		_bWriteAVCSeqHeader = 1;
	}

	if (_bWriteAVCSeqHeader == 0) {
		return 1;
	}

	WriteH264Frame(pNalu, nNaluSize, nTimeStamp);

	return 1;
}

#endif

