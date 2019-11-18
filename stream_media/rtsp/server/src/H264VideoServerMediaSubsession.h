#include "liveMedia.hh"  
#include "BasicUsageEnvironment.hh"  
#include "GroupsockHelper.hh"  
  
#include "OnDemandServerMediaSubsession.hh"  
#include "H264VideoSource.h"  



class H264VideoServerMediaSubsession : public OnDemandServerMediaSubsession  
{  
public:  
    H264VideoServerMediaSubsession(UsageEnvironment &env);  
    ~H264VideoServerMediaSubsession(void);  
  
public:  
    virtual char const *getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource);  
    virtual FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate); // "estBitrate" is the stream's estimated bitrate, in kbps  
    virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource);  
  
    static H264VideoServerMediaSubsession *createNew(UsageEnvironment &env);  
  
    void afterPlayingDummy1();
    void checkForAuxSDPLine1();

    void setDoneFlag() { m_fDoneFlag = ~0; }
  
private:  
    FramedSource    *m_pSource;  
    char            *m_fAuxSDPLine;  
    RTPSink         *m_pDummyRTPSink;  
    char            m_fDoneFlag;  
};


