#include <string>

#include "base/Lock.h"
#include "base/MsgQueue.h"



using namespace std;

namespace Base {

CMsgQueue::CMsgQueue(string name)
    : m_name(name)
{
}

CMsgQueue::~CMsgQueue()
{
}

bool CMsgQueue::RecvMsg(unsigned int &msg, void **data)
{
    Element element;

    CLock lock(m_mutex);

    if(m_queue.empty()) {
        return false;
    }

    element = m_queue.front();
    m_queue.pop();

    msg = element.msg;
    *data = element.data;

    return true;
}

bool CMsgQueue::SendMsg(unsigned int msg, void *data)
{
    Element element;

    CLock lock(m_mutex);

    element.msg = msg;
    element.data = data;

    m_queue.push(element);

    return true;
}

} // end namespace


