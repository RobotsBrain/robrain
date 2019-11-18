#ifndef __VIDEOTAG_H__
#define __VIDEOTAG_H__


#include "Flv.h"
#include "Tag.h"



class CVideoTag : public CTag 
{
public:
	CVideoTag(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlv *pFlv);
	~CVideoTag();

	void PrintVideoTag();

private:
	int ParseH264Tag();
	int ParseH264Configuration(u_char *pTagData);
	int ParseNalu(u_char *pTagData);

private:
	CFlv *m_pFlv;

	int m_nFrameType;
	int m_nCodecID;
	int m_nAVCPacketType;
	int m_nCompositionTime;
	int m_nNaluSize;
};

#endif