#include <string.h>
#include <stdio.h>

#include "Common.h"
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

#if 0
int CFlvEncoder::ConvertAAC(char *pAAC, int nAACFrameSize, unsigned int nTimeStamp)
{
	if (pAAC == NULL || nAACFrameSize <= 7) {
		return 0;
	}

	if (_pAudioSpecificConfig == NULL) {
		_pAudioSpecificConfig = new unsigned char[2];
		_nAudioConfigSize = 2;

		unsigned char *p = (unsigned char *)pAAC;
		_aacProfile = (p[2] >> 6) + 1;
		_sampleRateIndex = (p[2] >> 2) & 0x0f;
		_channelConfig = ((p[2] & 0x01) << 2) + (p[3] >> 6);

		_pAudioSpecificConfig[0] = (_aacProfile << 3) + (_sampleRateIndex >> 1);
		_pAudioSpecificConfig[1] = ((_sampleRateIndex & 0x01) << 7) + (_channelConfig << 3);
	}

	if (_pAudioSpecificConfig != NULL & _bWriteAACSeqHeader == 0) {
		WriteAACHeader(nTimeStamp);
		_bWriteAACSeqHeader = 1;
	}

	if (_bWriteAACSeqHeader == 0) {
		return 1;
	}

	WriteAACFrame(pAAC, nAACFrameSize, nTimeStamp);

	return 1;
}

void CFlvEncoder::WriteAACHeader(unsigned int nTimeStamp)
{
	u4 prev_u4(_nPrevTagSize);
	_fileOut.write((char *)prev_u4._u, 4);

	char cTagType = 0x08;
	_fileOut.write(&cTagType, 1);
	int nDataSize = 1 + 1 + 2;

	u3 datasize_u3(nDataSize);
	_fileOut.write((char *)datasize_u3._u, 3);

	u3 tt_u3(nTimeStamp);
	_fileOut.write((char *)tt_u3._u, 3);

	unsigned char cTTex = nTimeStamp >> 24;
	_fileOut.write((char *)&cTTex, 1);

	u3 sid_u3(_nStreamID);
	_fileOut.write((char *)sid_u3._u, 3);

	unsigned char cAudioParam = 0xAF;
	_fileOut.write((char *)&cAudioParam, 1);
	unsigned char cAACPacketType = 0;	/* seq header */
	_fileOut.write((char *)&cAACPacketType, 1);

	_fileOut.write((char *)_pAudioSpecificConfig, 2);

	_nPrevTagSize = 11 + nDataSize;
}

void CFlvEncoder::WriteAACFrame(char *pFrame, int nFrameSize, unsigned int nTimeStamp)
{
	u4 prev_u4(_nPrevTagSize);
	Write(prev_u4);

	Write(0x08);
	int nDataSize;
	nDataSize = 1 + 1 + (nFrameSize - 7);
	u3 datasize_u3(nDataSize);
	Write(datasize_u3);
	u3 tt_u3(nTimeStamp);
	Write(tt_u3);
	Write((unsigned char)(nTimeStamp >> 24));

	u3 sid(_nStreamID);
	Write(sid);

	unsigned char cAudioParam = 0xAF;
	_fileOut.write((char *)&cAudioParam, 1);
	unsigned char cAACPacketType = 1;	/* AAC raw data */
	_fileOut.write((char *)&cAACPacketType, 1);

	_fileOut.write((char *)pFrame + 7, nFrameSize - 7);

	_nPrevTagSize = 11 + nDataSize;
}
#endif

