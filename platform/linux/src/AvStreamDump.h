#ifndef __AVSTREAMDUMP_H__
#define __AVSTREAMDUMP_H__

#include "base/ThreadLoop.h"



class CAvStreamDump: public Base::CThreadLoop
{
public:
	CAvStreamDump();
	~CAvStreamDump();
	
	bool Start();
	bool Stop();
	
private:
	virtual void EventHandleLoop();
};


#endif


