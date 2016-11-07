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
	int ConvertH264(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp);

private:
	void SetupTagHeader(int type, int datasize, uint32_t nTimeStamp, int streamid,
						TagHeader &header, u_char *ptag);

	int WriteAACHeader(u_char *pAAC, uint32_t nTimeStamp);
	void WriteAACFrame(u_char *pFrame, int nFrameSize, uint32_t nTimeStamp);

	void WriteH264Header(uint32_t nTimeStamp);
	void WriteH264Frame(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp);
	void WriteH264EndofSeq();

private:
	bool 	m_bHaveAudio;
	bool 	m_bHaveVideo;

	bool 	m_bWriteAACSeqHeader;
	bool 	m_bWriteAVCSeqHeader;

	u_char 	*m_pSPS;
	int 	m_nSPSSize;
	u_char  *m_pPPS;
	int		m_nPPSSize;

	int 	m_nVideoTimeStamp;
};

#endif