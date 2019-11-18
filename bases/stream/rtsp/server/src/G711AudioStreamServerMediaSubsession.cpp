#include "SimpleRTPSink.hh"

#include "G711AudioStreamServerMediaSubsession.h"
#include "G711AudioStreamSource.h"



G711AudioStreamServerMediaSubsession* G711AudioStreamServerMediaSubsession::createNew(UsageEnvironment& env, Boolean reuseFirstSource) 
{
    return new G711AudioStreamServerMediaSubsession(env, reuseFirstSource);
}

G711AudioStreamServerMediaSubsession::G711AudioStreamServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource)
: OnDemandServerMediaSubsession(env, reuseFirstSource)
{
}

G711AudioStreamServerMediaSubsession::~G711AudioStreamServerMediaSubsession() 
{
}

FramedSource* G711AudioStreamServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) 
{
    FramedSource* resultSource = NULL;

    do {
        G711AudioStreamSource* g711Source = G711AudioStreamSource::createNew(envir());
        if (g711Source == NULL) {
            break;
        }

        // Get attributes of the audio source:

        fBitsPerSample = g711Source->bitsPerSample();
        if (!(fBitsPerSample == 4 || fBitsPerSample == 8 || fBitsPerSample == 16)) {
            envir() << "The input file contains " << fBitsPerSample << " bit-per-sample audio, which we don't handle\n";
            break;
        }

        fSamplingFrequency = g711Source->samplingFrequency();
        fNumChannels = g711Source->numChannels();
        unsigned bitsPerSecond = fSamplingFrequency*fBitsPerSample*fNumChannels;

        resultSource = g711Source;

        estBitrate = (bitsPerSecond + 500)/1000; // kbps

        return resultSource;
    } while (0);

    // An error occurred:
    Medium::close(resultSource);

    return NULL;
}

RTPSink* G711AudioStreamServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock,
                                                                unsigned char rtpPayloadTypeIfDynamic, FramedSource* /*inputSource*/) 
{
    const char *mimeType = "PCMA";
    unsigned char payloadFormatCode;

    if (fSamplingFrequency == 8000 && fNumChannels == 1) {
        payloadFormatCode = 8; // a static RTP payload type
    } else {
        payloadFormatCode = rtpPayloadTypeIfDynamic;
    }

    return SimpleRTPSink::createNew(envir(), rtpGroupsock, payloadFormatCode, fSamplingFrequency,
                                    "audio", mimeType, fNumChannels);
}


