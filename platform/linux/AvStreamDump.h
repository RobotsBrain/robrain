#ifndef __AVSTREAMDUMP_H__
#define __AVSTREAMDUMP_H__

#include "ThreadLoop.h"



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


