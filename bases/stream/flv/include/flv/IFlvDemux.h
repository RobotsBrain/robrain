#ifndef __IFLVDEMUX_H__
#define __IFLVDEMUX_H__

#include <stdint.h>


namespace Flv {

class IAVDataCallback
{
public:
	virtual ~IAVDataCallback() {};

	virtual void SpsPps(const void *context, const char *sps, const int sps_size, 
						const char *pps, const int pps_size) = 0;
	virtual void H264Nalu(const void *context, const char *nalu, const int size, const uint32_t timestamp) = 0;

	virtual void AudioSpecific(const void *context, const int profile, const int sampleRateIndex, const int channel) = 0;
	virtual void AacPacket(const void *context, const char *aac, const int size, const uint32_t timestamp) = 0;
};



class IFlvDemux
{
public:
	static IFlvDemux *Create();

	virtual ~IFlvDemux() {};

	virtual void Destroy() = 0;

	virtual void SetCallback(IAVDataCallback *pcbk, void *context) = 0;

	virtual int Parser(u_char *pbuf, int size, int &uselen) = 0;
};

} // end namespace

#endif