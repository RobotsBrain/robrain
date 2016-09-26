#include <time.h>
#include <stdio.h>  
#include <signal.h>  
#include <time.h>  
#include <string.h>  
#include <stdlib.h>  
#include <unistd.h>  

  
  
void timer_thread(union sigval v)  
{  
    printf("timer_thread function! %d\n", v.sival_int);  
}  
  
int main(int argc, char **argv)  
{  
    timer_t timerid;  
    struct sigevent evp;

    memset(&evp, 0, sizeof(struct sigevent));
  
    evp.sigev_value.sival_int = 111;            //也是标识定时器的，这和timerid有什么区别？回调函数可以获得  
    evp.sigev_notify = SIGEV_THREAD;            //线程通知的方式，派驻新线程  
    evp.sigev_notify_function = timer_thread;       //线程函数地址  
  
    if (timer_create(CLOCK_REALTIME, &evp, &timerid) == -1) {  
        perror("fail to timer_create");  
        exit(-1);  
    }  
  
    struct itimerspec it;  
    it.it_interval.tv_sec = 1;  
    it.it_interval.tv_nsec = 0;  
    it.it_value.tv_sec = 1;  
    it.it_value.tv_nsec = 0;  
  
    if (timer_settime(timerid, 0, &it, NULL) == -1) {  
        perror("fail to timer_settime");  
        exit(-1);  
    }  
  
    pause();  
  
    return 0;  
}


 
