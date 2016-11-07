#ifndef __FLV_H__
#define __FLV_H__

#include <sys/types.h>
#include <stdint.h>


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

	void SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig);
	void GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig);

	void SetNalUnitLength(int len);
	void GetNalUnitLength(int &len);
	
private:
	int 			m_aacProfile;
	int 			m_sampleRateIndex;
	int 			m_channelConfig;
	int 			m_nNalUnitLength;
};

#endif