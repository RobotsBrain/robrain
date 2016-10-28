#include "base/Log.h"


#include "H264VideoServerMediaSubsession.h"  



H264VideoServerMediaSubsession::H264VideoServerMediaSubsession(UsageEnvironment &env, 
                                                                FramedSource *source)
: OnDemandServerMediaSubsession(env, True)
, m_fAuxSDPLine(NULL)
, m_fDoneFlag(0)
{
    m_pSource = source;
}

H264VideoServerMediaSubsession::~H264VideoServerMediaSubsession(void)  
{
    if (m_fAuxSDPLine) {
        free(m_fAuxSDPLine);
        m_fAuxSDPLine = NULL;
    }
}

H264VideoServerMediaSubsession *H264VideoServerMediaSubsession::createNew(UsageEnvironment &env, FramedSource *source)  
{
    return new H264VideoServerMediaSubsession(env, source);  
}

static void afterPlayingDummy(void *clientData)
{
    H264VideoServerMediaSubsession* subsess = (H264VideoServerMediaSubsession*)clientData;
    subsess->afterPlayingDummy1();
}

void H264VideoServerMediaSubsession::afterPlayingDummy1()
{
    // Unschedule any pending 'checking' task:
    envir().taskScheduler().unscheduleDelayedTask(nextTask());
    // Signal the event loop that we're done:
    setDoneFlag();
}

static void checkForAuxSDPLine(void *clientData)  
{  
    H264VideoServerMediaSubsession *subsess = (H264VideoServerMediaSubsession *)clientData;
  
    subsess->checkForAuxSDPLine1();
}

void H264VideoServerMediaSubsession::checkForAuxSDPLine1()
{
    DEBUG("Begin___\n");

    nextTask() = NULL;

    char const* dasl = NULL;

    if (m_fAuxSDPLine != NULL) {
        // Signal the event loop that we're done:
        setDoneFlag();
    } else if (m_pDummyRTPSink != NULL && (dasl = m_pDummyRTPSink->auxSDPLine()) != NULL) {
        m_fAuxSDPLine = strDup(dasl);
        m_pDummyRTPSink = NULL;

        // Signal the event loop that we're done:
        setDoneFlag();
    } else {
        double delay = 1000.0 / (FRAME_PER_SEC);  // ms
        int to_delay = delay * 1000;  // us

        nextTask() = envir().taskScheduler().scheduleDelayedTask(to_delay, checkForAuxSDPLine, this);
    }

    DEBUG("End___\n");

    return;
}

char const *H264VideoServerMediaSubsession::getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource)
{
    DEBUG("Begin___\n");

    if (m_fAuxSDPLine != NULL) {
        return m_fAuxSDPLine;
    }
  
    if(m_pDummyRTPSink == NULL) {
        m_pDummyRTPSink = rtpSink;
 
        m_pDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);
      
        checkForAuxSDPLine(this);
    }
  
    envir().taskScheduler().doEventLoop(&m_fDoneFlag);
    
    DEBUG("End___, sdp: %s\n", m_fAuxSDPLine);

    return m_fAuxSDPLine;  
}

FramedSource *H264VideoServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate)
{
    estBitrate = 500; // kbps, estimate

    // create stream source
    H264VideoSource *streamSource = new H264VideoSource(envir());
    if(streamSource == NULL) {
        return NULL;
    }

    return H264VideoStreamFramer::createNew(envir(), streamSource);
}

RTPSink *H264VideoServerMediaSubsession::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource)
{
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}


