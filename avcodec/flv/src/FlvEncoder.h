#ifndef __FLVENCODER_H__
#define __FLVENCODER_H__

#include "Flv.h"



class CFlvEncoder
{
public:
	CFlvEncoder();
	~CFlvEncoder();
	
	bool Start(bool bHaveAudio, bool bHaveVideo);
	bool Stop();

private:
	bool 	m_bHaveAudio;
	bool 	m_bHaveVideo;

	FlvHeader m_FlvHeader;
};

#endif