#ifndef __VIDEOTAG_H__
#define __VIDEOTAG_H__


#include "Tag.h"



class CVideoTag : public CTag 
{
public:
	CVideoTag(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlvParser *pParser);

	int ParseH264Tag();
	int ParseH264Configuration(u_char *pTagData);
	int ParseNalu(u_char *pTagData);

private:
	int m_nFrameType;
	int m_nCodecID;
};

#endif