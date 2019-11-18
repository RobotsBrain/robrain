#ifndef __AVFRAMERINGBUFFER_H__
#define __AVFRAMERINGBUFFER_H__

#include <pthread.h>

#include "base/CircularBuffer.h"

#include "AvPacket.h"



class CAvFrameRingBuffer: public Base::CCircularBuffer
{
public:
	static CAvFrameRingBuffer *Instance();

	int WriteFrame(AvPacket packet, char *data, int len);
	int ReadFrame(AvPacket &packet, char *data, int len);
	
private:
	CAvFrameRingBuffer();
	~CAvFrameRingBuffer();
	
private:
	pthread_mutex_t m_mutex;
	pthread_cond_t 	m_cond;
};

#endif
