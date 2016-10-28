#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#include "H264VideoSource.h"



#define FIFO_NAME     "/tmp/H264_fifo"
#define BUFFER_SIZE   PIPE_BUF
#define REV_BUF_SIZE  (1024*1024)



H264VideoSource::H264VideoSource(UsageEnvironment & env)
: FramedSource(env)
, m_pToken(0)
, m_pFrameBuffer(0)
, m_hFifo(0)
{
    m_hFifo = open(FIFO_NAME, O_RDONLY);
    if(m_hFifo == -1) {
        printf("open fifo fail!\n");
        return;
    }

    printf("open fifo(%d) success!\n", m_hFifo);  
      
    m_pFrameBuffer = new char[REV_BUF_SIZE];  
    if(m_pFrameBuffer == NULL) {
        printf("malloc data buffer failed\n");
        return;
    }

    memset(m_pFrameBuffer, 0, REV_BUF_SIZE);
}

H264VideoSource::~H264VideoSource(void)
{
    if(m_hFifo) {
        ::close(m_hFifo);
    }

    envir().taskScheduler().unscheduleDelayedTask(m_pToken);
  
    if(m_pFrameBuffer) {
        delete[] m_pFrameBuffer;
        m_pFrameBuffer = NULL;
    }

    printf("rtsp connection closed\n");
}

void H264VideoSource::doGetNextFrame()
{
    // 根据 fps，计算等待时间
    double delay = 1000.0 / (FRAME_PER_SEC * 2);  // ms
    int to_delay = delay * 1000;  // us

    m_pToken = envir().taskScheduler().scheduleDelayedTask(to_delay, getNextFrame, this);
}

unsigned int H264VideoSource::maxFrameSize() const
{
    return 150000;
}

void H264VideoSource::getNextFrame(void *ptr)
{
   ((H264VideoSource *)ptr)->GetFrameData();
}

void H264VideoSource::GetFrameData()
{
    int len = 0;
    unsigned char buffer[BUFFER_SIZE] = {0};

    gettimeofday(&fPresentationTime, 0);

    fFrameSize = 0;

    while((len = read(m_hFifo, buffer, BUFFER_SIZE)) > 0) {
        memcpy(m_pFrameBuffer + fFrameSize, buffer, len);
        fFrameSize += len;
    }  

    memcpy(fTo, m_pFrameBuffer, fFrameSize);
  
    if (fFrameSize > fMaxSize) {
        fNumTruncatedBytes = fFrameSize - fMaxSize;
        fFrameSize = fMaxSize;
    } else {
        fNumTruncatedBytes = 0;
    }

    afterGetting(this);

    return;
}


