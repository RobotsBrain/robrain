#ifndef __SOCKET_H__
#define __SOCKET_H__


#include "base/ThreadLoop.h"



namespace Rtsp {

class CSocket : public Base::CThreadLoop
{
public:
	CSocket();
	~CSocket();

	bool Start(const char *host, int port);
	bool Stop();

private:
	void EventHandleLoop();

private:
	int m_rtspfd;
	
};

} // end namespace

#endif


