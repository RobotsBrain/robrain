#include <sys/time.h>
#include <sys/prctl.h>
#include <errno.h>

#include "Log.h"
#include "ThreadLoop.h"



namespace Base {


CThreadLoop::CThreadLoop(std::string name)
: m_name(name)
, m_start(false)
{
}

CThreadLoop::~CThreadLoop()
{
	StopThread();
}
	
bool CThreadLoop::StartThread()
{
	int res = false;

	if(m_start) {
		INFO("thread(%s) has started!\n", m_name.c_str());
		return true;
	}

	pthread_mutex_init(&m_lock, NULL);
	pthread_cond_init(&m_condition, NULL);

	pthread_attr_init(&m_attr);  
     
    if(pthread_create(&m_tid, &m_attr, ThreadProc, this) == -1) {  
 		ERROR("can not create thread\n");
 		res = false;
    } else {
    	m_start = true;
    	INFO("start thread(%s) success!\n", m_name.c_str());
    }

    return res;
}

bool CThreadLoop::StopThread()
{
	if(!m_start) {
		INFO("thread(%s) has stopped!\n", m_name.c_str());
		return true;
	}

	pthread_cond_broadcast(&m_condition);

	pthread_join(m_tid, NULL);
    pthread_attr_destroy(&m_attr);

    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_condition);

    m_start = false;

    INFO("stop thread(%s) success!\n", m_name.c_str());

	return true;
}

int CThreadLoop::WaitForSleep(int timeout_ms)
{
	int ret = -1;
	struct timespec abstime;
	struct timeval now;

	gettimeofday(&now, NULL);

	int nsec = now.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
	abstime.tv_nsec = nsec % 1000000000;
	abstime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout_ms / 1000;

	int err = pthread_cond_timedwait(&m_condition, &m_lock, &abstime);
	if(err == ETIMEDOUT) {
		ret = 0;
	}
	
	return ret;
}

void CThreadLoop::EventHandleLoop()
{
	return;
}

void *CThreadLoop::ThreadProc(void *argv)
{
	CThreadLoop *thiz = (CThreadLoop *)argv;

	if(thiz != NULL) {
		prctl(PR_SET_NAME, thiz->m_name.c_str());
		thiz->EventHandleLoop();
	}

	return NULL;
}

} // end namespace


