#ifndef __AUDIOTAG_H__
#define __AUDIOTAG_H__

#include "Flv.h"
#include "Tag.h"



class CAudioTag : public CTag 
{
public:
	CAudioTag(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlv *pFlv);
	~CAudioTag();
	
	void PrintAudioTag();

private:
	int ParseAACTag();
	int ParseAudioSpecificConfig();
	int ParseRawAAC(u_char *pTagData);

private:
	CFlv *m_pFlv;

	int m_nSoundFormat;
	int m_nSoundRate;
	int m_nSoundSize;
	int m_nSoundType;	
};


#endif

