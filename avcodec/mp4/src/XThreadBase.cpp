#include "XThreadBase.h"



XThreadBase::XThreadBase() 
{
	sem_init(&m_semWaitForStop, 0, 1);

	m_thread = -1;

	m_bThreadStopped = true;
}

XThreadBase::~XThreadBase() 
{
	sem_destroy(&m_semWaitForStop);
}

bool XThreadBase::StartThread() 
{
	if (pthread_create(&m_thread, NULL, InitThreadProc, (void *)this) != 0)
		return false;
	
	sem_wait(&m_semWaitForStop);
	m_bThreadStopped = false;
	
	return true;
}

void XThreadBase::WaitForStop() 
{
	if (m_bThreadStopped == false) {
		sem_wait(&m_semWaitForStop);
		m_bThreadStopped = true;
		pthread_join(m_thread, NULL);
	}
}

unsigned long XThreadBase::ThreadProc() 
{
	ThreadProcMain();
	sem_post(&m_semWaitForStop);
	return 0;
}

 
 
