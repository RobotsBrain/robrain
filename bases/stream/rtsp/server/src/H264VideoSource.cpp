#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#include "base/Log.h"

#include "H264VideoSource.h"



H264VideoSource::H264VideoSource(UsageEnvironment &env)
: FramedSource(env)
, m_pToken(NULL)
{
    DEBUG("\n");

    m_fp = fopen("test.h264", "rb");
}

H264VideoSource::~H264VideoSource()
{
    DEBUG("Begin___\n");

    fclose(m_fp);
    m_fp = NULL;

    envir().taskScheduler().unscheduleDelayedTask(m_pToken);

    DEBUG("End___\n");
}

void H264VideoSource::doGetNextFrame()
{
    DEBUG("\n");

    // 根据 fps，计算等待时间
    double delay = 1000.0 / (FRAME_PER_SEC * 2);  // ms
    int to_delay = delay * 1000;  // us

    m_pToken = envir().taskScheduler().scheduleDelayedTask(to_delay, getNextFrame, this);
}

unsigned int H264VideoSource::maxFrameSize() const
{
    DEBUG("\n");

    return 150000;
}

void H264VideoSource::getNextFrame(void *ptr)
{
   ((H264VideoSource *)ptr)->GetFrameData();
}

long filesize(FILE *stream)
{
    long curpos, length;

    curpos = ftell(stream);
    fseek(stream, 0L, SEEK_END);
    length = ftell(stream);
    fseek(stream, curpos, SEEK_SET);

    return length;
}

void H264VideoSource::GetFrameData()
{
    DEBUG("Begin___\n");

    gettimeofday(&fPresentationTime, 0);

    fFrameSize = 0;

    if(feof(m_fp)) {
        fseek(m_fp, 0L, SEEK_SET);
    }

    if(filesize(m_fp) > fMaxSize) {
        fFrameSize = fread(fTo, 1, fMaxSize, m_fp); 
    } else {
        fFrameSize = fread(fTo, 1, filesize(m_fp), m_fp);
        fseek(m_fp, 0, SEEK_SET);
    }

    // Inform the downstream object that it has data:
    FramedSource::afterGetting(this);

    DEBUG("End___, fFrameSize: %d, fMaxSize: %d, fNumTruncatedBytes: %d\n", 
        fFrameSize, fMaxSize, fNumTruncatedBytes);

    return;
}


