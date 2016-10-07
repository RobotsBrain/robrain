#include "AvFrameRingBuffer.h"



CAvFrameRingBuffer *CAvFrameRingBuffer::Instance()
{
	static CAvFrameRingBuffer inst;
	return &inst;
}

CAvFrameRingBuffer::CAvFrameRingBuffer()
: Base::CCircularBuffer(8 * 1024 * 1024)
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
}

CAvFrameRingBuffer::~CAvFrameRingBuffer()
{
	pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

int CAvFrameRingBuffer::WriteFrame(AvPacket packet, char *data, int len)
{
	pthread_mutex_lock(&m_mutex);

	while((sizeof(AvPacket) + len) > Capacity() - Size()) {
		pthread_cond_wait(&m_cond, &m_mutex);
	}

	Write((const char *)&packet, sizeof(AvPacket));
	Write(data, len);

	pthread_cond_broadcast(&m_cond);

    pthread_mutex_unlock(&m_mutex);

    return 0;
}

int CAvFrameRingBuffer::ReadFrame(AvPacket &packet, char *data, int len)
{
	pthread_mutex_lock(&m_mutex);

	while(Size() < sizeof(AvPacket)) {
		pthread_cond_wait(&m_cond, &m_mutex);
	}

	Read((char *)&packet, sizeof(AvPacket));
	Read(data, packet.len);

	pthread_cond_broadcast(&m_cond);

    pthread_mutex_unlock(&m_mutex);

    return 0;
}


