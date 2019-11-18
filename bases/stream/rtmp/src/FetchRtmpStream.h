#ifndef __FETCHRTMPSTREAM_H__
#define __FETCHRTMPSTREAM_H__

#include <string>

#include "librtmp/log.h"
#include "librtmp/rtmp.h"



typedef void (*PACKETCALLBACK)(void *context, char *data, int size);


namespace Rtmp {

class CFetchRtmpStream
{
public:
	CFetchRtmpStream();
	~CFetchRtmpStream();

	void SetParams(void *context, PACKETCALLBACK pcbk, std::string url);

	void Loop();
	
private:
	void 			*m_context;
	PACKETCALLBACK 	m_pcbk;
	std::string		m_url;
	RTMP 			*m_rtmp;
};

} // end namespace

#endif