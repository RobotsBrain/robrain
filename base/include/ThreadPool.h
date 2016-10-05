#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "DoubleLinkList.h"



typedef void * (* THREADPOOLCBKFUNC)(void * argv);


namespace Base {

class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();
	
	int Create(int nthreads);
	void Destroy();
	void AddTask(THREADPOOLCBKFUNC func, void * argv);

private:
	void EventProcess();
	static void *ThreadProc(void *argv);

private:
	typedef struct threadpool_st {
		int num;	/*number of threads*/
	    int shutdown;

		CDoubleLinkList thread_list;
		CDoubleLinkList task_list;

		pthread_mutex_t lock;
	    pthread_cond_t condition;
	} Threadpool;

	Threadpool m_pool;
};

} // end namespace

#endif


