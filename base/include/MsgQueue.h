#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__


#include <string>
#include <queue>

#include "Lock.h"



namespace Base {

class CMsgQueue
{
public:
	CMsgQueue(std::string name = "");
	~CMsgQueue();
	
	bool RecvMsg(unsigned int &msg, void **data);
	bool SendMsg(unsigned int msg, void *data);

	const char *GetName() const {
		return m_name.c_str();
	}

private:
	typedef struct {
		unsigned int msg;
		void 		 *data;
	} Element;

	std::string 		m_name;
	std::queue<Element> m_queue;
	CMutex				m_mutex;
};

} // end namespace


#endif


