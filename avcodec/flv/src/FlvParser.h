#ifndef FLVPARSER_H
#define FLVPARSER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "Tag.h"



typedef struct {
	int nVersion;
	int bHaveVideo;
	int bHaveAudio;
	int nHeadSize;

	u_char *pFlvHeader;
} FlvHeader;

class CFlvParser {
public:
	CFlvParser();
	virtual ~CFlvParser();

	int Parse(u_char *pBuf, int nBufSize, int &nUsedLen);

	void SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig);
	void GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig);

	void SetNalUnitLength(int len);
	void GetNalUnitLength(int &len);

	void GetTags(std::vector <CTag *> &vpTag);
	void GetFlvHeader(FlvHeader &flvHeader);

private:
	FlvHeader *CreateFlvHeader(u_char *pBuf);
	void DestroyFlvHeader();

	CTag *CreateTag(u_char *pBuf, int nLeftLen);
	void DestroyTag();

private:
	FlvHeader 				*m_pFlvHeader;
	std::vector <CTag *>	m_vpTag;

	int 			m_aacProfile;
	int 			m_sampleRateIndex;
	int 			m_channelConfig;
	int 			m_nNalUnitLength;
};

#endif // FLVPARSER_H
