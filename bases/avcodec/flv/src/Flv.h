#ifndef __FLV_H__
#define __FLV_H__

#include <sys/types.h>
#include <stdint.h>
#include <vector>

#include "Tag.h"



typedef struct {
	int nVersion;
	int bHaveVideo;
	int bHaveAudio;
	int nHeadSize;

	u_char *pFlvHeader;
} FlvHeader;

class CFlv
{
public:
	CFlv();
	~CFlv();

	void WriteHeaderData(FlvHeader Header, u_char *pBuf);
	void WriteTag(CTag *pTag);

	void SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig);
	void GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig);

	void SetNalUnitLength(int len);
	void GetNalUnitLength(int &len);
	
	void GetTags(std::vector <CTag *> &vpTag);
	void GetFlvHeader(FlvHeader &flvHeader);

	void PrintFlvHeader();

	void DestroyFlvHeader();
	void DestroyTag();

private:
	FlvHeader 				m_pFlvHeader;
	std::vector <CTag *>	m_vpTag;

	int 			m_aacProfile;
	int 			m_sampleRateIndex;
	int 			m_channelConfig;
	int 			m_nNalUnitLength;
};

#endif