#ifndef __FLVENCODER_H__
#define __FLVENCODER_H__

#include <vector>

#include "Tag.h"
#include "Flv.h"



class CFlvEncoder : public CFlv
{
public:
	CFlvEncoder();
	~CFlvEncoder();
	
	bool Start(bool bHaveAudio, bool bHaveVideo);
	bool Stop();

	int ConvertAAC(u_char *pAAC, int nAACFrameSize, uint32_t nTimeStamp);

private:
	int WriteAACHeader(u_char *pAAC, uint32_t nTimeStamp);
	void WriteAACFrame(u_char *pFrame, int nFrameSize, uint32_t nTimeStamp);

private:
	bool 	m_bWriteAACSeqHeader;
};

#endif