#include "ThreadPool.h"



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
	memset(m_pool, 0, sizeof(m_pool));
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

	for (int i = 0; i < nthreads; i++) {
		thread *tempThread = (Thread *)malloc(sizeof(Thread));
		if(tempThread == NULL) {
			continue;
		}

		pthread_create(&tempThread->tid, NULL, ThreadProc, this);

		m_pool.thread_list.PushBack(&tempThread->elem);
	}

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

	
    for (e = rb_dlist_begin(&pool->thread_list); e != rb_dlist_end(&pool->thread_list); e = rb_dlist_next(e)) {
		thread *joinThread = container_of(e, thread, elem);
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

		pthread_mutex_lock(&pool->lock);

		while(rb_dlist_empty(&pool->task_list)) {
			pthread_cond_wait(&pool->condition, &pool->lock);
			
			if (pool->shutdown) {
				pthread_mutex_unlock(&pool->lock);
				pthread_exit(NULL);
			}
		}

		threadpool_task *initTask = container_of(m_pool.task_list.PopFront(), threadpool_task, elem);

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

void CThreadPool::AddTask(thread_pool_callback_func function, void * data) 
{
    pthread_mutex_lock(&pool->lock);

    if (pool->shutdown) {
		pthread_mutex_unlock(&pool->lock);
		return;
	}
    
    threadpool_task *newTask = (threadpool_task *)malloc(sizeof(threadpool_task));
    newTask->function = function;
    newTask->data = data;
    rb_dlist_push_back(&pool->task_list, &newTask->elem);
    pthread_cond_signal(&pool->condition);
    pthread_mutex_unlock(&pool->lock);

    return;
}

} // end namespace


