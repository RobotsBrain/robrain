#include "base/ThreadPool.h"



namespace Base {

typedef struct threadpool_task_st {
	THREADPOOLCBKFUNC function;
    void *data;
	DLListElem elem;
} Task;

typedef struct thread_st {
    pthread_t tid;
    DLListElem elem;
} Thread;

CThreadPool::CThreadPool()
{
}

CThreadPool::~CThreadPool()
{
}

int CThreadPool::Create(int nthreads)
{
	m_pool.num = nthreads;
	m_pool.shutdown = 0;

	pthread_mutex_init(&m_pool.lock, NULL);
	pthread_cond_init(&m_pool.condition, NULL);

	pthread_mutex_lock(&m_pool.lock);

	for (int i = 0; i < nthreads; i++) {
		Thread *tempThread = (Thread *)malloc(sizeof(Thread));
		if(tempThread == NULL) {
			continue;
		}

		pthread_create(&tempThread->tid, NULL, ThreadProc, this);

		m_pool.thread_list.PushBack(&tempThread->elem);
	}

	pthread_mutex_unlock(&m_pool.lock);

	return 0;
}

void CThreadPool::Destroy() 
{
    pthread_mutex_lock(&m_pool.lock);
	m_pool.shutdown = 1;

	if ((pthread_cond_broadcast(&m_pool.condition) != 0) 
		|| (pthread_mutex_unlock(&m_pool.lock) != 0)) {
		return;
	}

	DLListElem *e = NULL;

	while((e = m_pool.thread_list.PopFront()) != NULL) {
		Thread *joinThread = container_of(e, Thread, elem);
		pthread_join(joinThread->tid, NULL);
	}

    pthread_mutex_unlock(&m_pool.lock);
    
    pthread_mutex_destroy(&m_pool.lock);
    pthread_cond_destroy(&m_pool.condition);

    return;
}

void CThreadPool::EventProcess()
{
	while(1) {

		pthread_mutex_lock(&m_pool.lock);

		while(m_pool.task_list.Empty()) {
			pthread_cond_wait(&m_pool.condition, &m_pool.lock);
		
			if (m_pool.shutdown) {
				pthread_mutex_unlock(&m_pool.lock);
				pthread_exit(NULL);
			}
		}

		Task *initTask = container_of(m_pool.task_list.PopFront(), Task, elem);

		pthread_mutex_unlock(&m_pool.lock);

		if(initTask != NULL) {
			(*(initTask->function))(initTask->data);
			free(initTask);
			initTask = NULL;
		}
    }

    return;
}

void *CThreadPool::ThreadProc(void *argv)
{    
	CThreadPool *thiz = (CThreadPool *)argv;

	if(thiz != NULL) {
		thiz->EventProcess();
	}
    
    return NULL;
}

void CThreadPool::AddTask(THREADPOOLCBKFUNC function, void * data) 
{
    pthread_mutex_lock(&m_pool.lock);

    if (m_pool.shutdown) {
		pthread_mutex_unlock(&m_pool.lock);
		return;
	}
   
    Task *newTask = (Task *)malloc(sizeof(Task));
    newTask->function = function;
    newTask->data = data;
    m_pool.task_list.PushBack(&newTask->elem);
    pthread_cond_signal(&m_pool.condition);
    pthread_mutex_unlock(&m_pool.lock);

    return;
}

} // end namespace


