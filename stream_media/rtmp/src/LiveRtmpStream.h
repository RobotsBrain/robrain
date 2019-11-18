#ifndef __LIVERTMPSTREAM_H__
#define __LIVERTMPSTREAM_H__

#include <string>

#include "librtmp/rtmp.h"


namespace Rtmp {

class CLiveRtmpStream
{
public:
	CLiveRtmpStream();
	~CLiveRtmpStream();

	void SetParams(std::string url);

	bool Start();
	bool Stop();

	int SendStream(char *buf, uint32_t size);

private:
	std::string		m_url;
	RTMP 			*m_rtmp;
};

} // end namespace

#endif