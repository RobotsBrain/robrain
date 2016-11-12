#ifndef __XTHREADBASE_H__
#define __XTHREADBASE_H__
	

#include <semaphore.h>
#include <pthread.h>

	
class XThreadBase 
{ 
public:
	XThreadBase(void);
	virtual ~XThreadBase(void);

public:
	bool StartThread(void);	
	void WaitForStop(void);
	
	static void *InitThreadProc(void *pObj) {
		((XThreadBase *) pObj)->ThreadProc();
		return 0; 
	}

	unsigned long ThreadProc(void);

protected:
	virtual void ThreadProcMain(void) = 0;

protected:
	pthread_t m_thread;
	sem_t m_semWaitForStop;
	bool m_bThreadStopped;
};
 
#endif

