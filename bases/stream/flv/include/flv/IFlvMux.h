#ifndef __IFLVMUX_H__
#define __IFLVMUX_H__

#include <stdint.h>


namespace Flv {

class IFlvDataCallback
{
public:
	virtual ~IFlvDataCallback() {};

	virtual void FlvPacket(const void *context, const u_char *data, const int size, const int pretagsize) = 0;
};

class IFlvMux
{
public:
	static IFlvMux *Create();

	virtual ~IFlvMux() {};

	virtual void Destroy() = 0;

	virtual void SetCallback(IFlvDataCallback *pcbk, void *context) = 0;

	virtual bool Start(bool bAudio, bool bVideo) = 0;
	virtual bool Stop() = 0;

	virtual int ConvertAAC(u_char *pAAC, int nAACFrameSize, uint32_t nTimeStamp) = 0;
	virtual int ConvertH264(u_char *pNalu, int nNaluSize, uint32_t nTimeStamp) = 0;
};

} // end namespace


#endif