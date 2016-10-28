#include <stdio.h>  
#include <pthread.h>  



class CRTSPStream  
{  
public:  
    CRTSPStream(void);
    ~CRTSPStream(void);
    
public:
    bool Init();
    void Uninit();

    bool SendH264File(const char *pFileName);
    int SendH264Data(const unsigned char *data, unsigned int size);
};


