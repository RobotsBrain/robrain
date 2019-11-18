#include "base/Lock.h"



namespace Base {

CMutex::CMutex()
{
    pthread_mutex_init(&m_mutex, NULL);
}

CMutex::~CMutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void CMutex::Lock() const
{
    pthread_mutex_lock(&m_mutex);
}

void CMutex::Unlock() const
{
    pthread_mutex_unlock(&m_mutex);
}

CLock::CLock(const CMutex& m)
    : m_lock(m)
{
    m_lock.Lock();
}

CLock::~CLock()
{
    m_lock.Unlock();
}

} // end namespace Base


