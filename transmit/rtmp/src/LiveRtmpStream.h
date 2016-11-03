#ifndef __LIVERTMPSTREAM_H__
#define __LIVERTMPSTREAM_H__



#include "base/ThreadLoop.h"

class CLiveRtmpStream : public Base::CThreadLoop
{
public:
	CLiveRtmpStream();
	~CLiveRtmpStream();

	bool Start();
	bool Stop();
	
	virtual void EventHandleLoop();
};

#endif