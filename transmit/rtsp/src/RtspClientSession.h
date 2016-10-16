#ifndef __RTSPCLIENTSESSION_H__
#define __RTSPCLIENTSESSION_H__

#include "base/ThreadLoop.h"



namespace Rtsp {

class CRtspClientSession : public Base::CThreadLoop
{
public:
	CRtspClientSession();
	~CRtspClientSession();
	
	bool Start(int fd);
	bool Stop();

private:
	void EventHandleLoop();

private:
	int 	m_clifd;
};

} // end namespace

#endif


