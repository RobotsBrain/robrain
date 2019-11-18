#include <sys/time.h>

#include "base/ThreadPool.h"



#define THREADPOOL_NUM 	5
#define TASK_NUM		30

void *task_function(void *argv)
{
    char buf[1024];
    struct timeval start, end;
    
	gettimeofday(&start, NULL);
	sleep(1);
    gettimeofday(&end, NULL);

    snprintf(buf, sizeof(buf),
            "Task %02d Thread %p start=%ld.%ld end=%ld.%ld",
            *(char *)argv, (void *)pthread_self(),
            start.tv_sec, start.tv_usec,
            end.tv_sec, end.tv_usec);

	printf("%s\n", buf);

    return NULL;
}

int main(int argc, char **argv)
{
	int i = 0;
	char *count = NULL;
	Base::CThreadPool pool;

	count = (char *)malloc(TASK_NUM);

	pool.Create(THREADPOOL_NUM);

	for(i = 0; i < TASK_NUM; i++) {
		count[i] = i;
		pool.AddTask(task_function, &count[i]);
	}

	sleep(30);

	pool.Destroy();

	return 0;
}





