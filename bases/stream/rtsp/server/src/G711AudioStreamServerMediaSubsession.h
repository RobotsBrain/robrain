#ifndef _G711_AUDIO_STREAM_SERVER_MEDIA_SUBSESSION_HH
#define _G711_AUDIO_STREAM_SERVER_MEDIA_SUBSESSION_HH

#include "OnDemandServerMediaSubsession.hh"



class G711AudioStreamServerMediaSubsession: public OnDemandServerMediaSubsession
{
public:
  static G711AudioStreamServerMediaSubsession*
  createNew(UsageEnvironment& env, Boolean reuseFirstSource);

protected:
  G711AudioStreamServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource);
      // called only by createNew();
  virtual ~G711AudioStreamServerMediaSubsession();

protected: // redefined virtual functions
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

protected:
  // The following parameters of the input stream are set after
  // "createNewStreamSource" is called:
  unsigned char fBitsPerSample;
  unsigned fSamplingFrequency;
  unsigned fNumChannels;
};

#endif
