#ifndef CONVERTER_H
#define CONVERTER_H

#include <fstream>

#include "Common.h"


class CConverter 
{
public:
	CConverter();
	virtual ~CConverter();

	int Open(std::string strFlvFile, int bHaveAudio = 0, int bHaveVideo = 1);
	int Close();

	int ConvertH264(char *pNalu, int nNaluSize, unsigned int nTimeStamp);
	int ConvertAAC(char *pAAC, int nAACFrameSize, unsigned int nTimeStamp);

private:
	void MakeFlvHeader(unsigned char *pFlvHeader);

	// h.264
	void WriteH264Header(unsigned int nTimeStamp);
	void WriteH264Frame(char *pNalu, int nNaluSize, unsigned int nTimeStamp);
	void WriteH264EndofSeq();

	// aac
	void WriteAACHeader(unsigned int nTimeStamp);
	void WriteAACFrame(char *pFrame, int nFrameSize, unsigned int nTimeStamp);

	void Write(unsigned char u) {
		_fileOut.write((char *)&u, 1);
	} 

	void Write(u4 u) {
		_fileOut.write((char *)u._u, 4);
	}
	
	void Write(u3 u) {
		_fileOut.write((char *)u._u, 3);
	}
	
	void Write(u2 u) {
		_fileOut.write((char *)u._u, 2);
	}

private:
	unsigned char _FlvHeader[9];
	unsigned char *_pSPS, *_pPPS;
	int _nSPSSize, _nPPSSize;
	int _bWriteAVCSeqHeader;
	int _nPrevTagSize;
	int _nStreamID;
	int _nVideoTimeStamp;

	unsigned char *_pAudioSpecificConfig;
	int _nAudioConfigSize;
	int _aacProfile;
	int _sampleRateIndex;
	int _channelConfig;
	int _bWriteAACSeqHeader;

	std::fstream _fileOut;
	int _bHaveAudio, _bHaveVideo;
};

#endif // CONVERTER_H
