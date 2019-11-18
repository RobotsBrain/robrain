#ifndef __FLVMUX_H__
#define __FLVMUX_H__

#include "flv/IFlvMux.h"
#include "flv/Flv.h"


namespace Flv {

class CFlvMux : public IFlvMux
{
public:
	CFlvMux();
	~CFlvMux();
	
	virtual void Destroy();

	virtual void SetCallback(IFlvDataCallback *pcbk, void *context);

	virtual bool Start(bool bAudio, bool bVideo);
	virtual bool Stop();

	virtual int ConvertAAC(u_char *pAAC, int nAACFrameSize, uint32_t nTimeStamp);
	virtual int ConvertH264(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp);

private:
	void SetupTagHeader(int type, int datasize, uint32_t nTimeStamp, int streamid,
						TagHeader &header, u_char *ptag);

	int WriteAACHeader(u_char *pAAC, uint32_t nTimeStamp);
	void WriteAACFrame(u_char *pFrame, int nFrameSize, uint32_t nTimeStamp);

	void WriteH264Header(uint32_t nTimeStamp);
	void WriteH264Frame(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp);
	void WriteH264EndofSeq();

private:
	bool 	m_bWriteAACSeqHeader;
	bool 	m_bWriteAVCSeqHeader;

	u_char 	*m_pSPS;
	int 	m_nSPSSize;
	u_char  *m_pPPS;
	int		m_nPPSSize;

	int 	m_nVideoTimeStamp;

	void 			*m_context;
	IFlvDataCallback *m_pFlvCbk;
};

} // end namespace

#endif