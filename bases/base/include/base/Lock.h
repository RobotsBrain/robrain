#ifndef __LOCK_H__
#define __LOCK_H__
  
#include <pthread.h>  


namespace Base {


class CMutex 
{  
public:  
    CMutex();  
    ~CMutex();  
  
    virtual void Lock() const;  
    virtual void Unlock() const;  
  
private:  
    mutable pthread_mutex_t m_mutex;  
};  
  

class CLock  
{  
public:  
    CLock(const CMutex&);  
    ~CLock();  
  
private:  
    const CMutex& m_lock;  
};  

} // end namespace Base
  
#endif  