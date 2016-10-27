#include <stdio.h>  
#include <pthread.h>  


typedef unsigned int SOCKET;  
typedef pthread_t    ThreadHandle;  
#define mSleep(ms)   usleep(ms*1000)  
#define FILEBUFSIZE (1024 * 1024)   
  
  
class CRTSPStream  
{  
public:  
    CRTSPStream(void);  
    ~CRTSPStream(void);  
    
public:  
    // 初始化  
    bool Init();  
    // 卸载  
    void Uninit();  
    // 发送H264文件  
    bool SendH264File(const char *pFileName);  
    // 发送H264数据帧  
    int SendH264Data(const unsigned char *data,unsigned int size);  
};  