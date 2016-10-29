#include "G711AudioStreamSource.h"



int audioOpen(void);
int audioClose(void);
int audioGetOneFrame(unsigned char *buf, unsigned int *size);

#if 0
static FILE *fp;
static int audioGetOneFrame(unsigned char *buf, unsigned int *size)
{
    int ret;

    ret = fread(buf, 1, 320, fp);
    if(ret < 0) {
        printf("ERR : %s : %d\n", __FILE__, __LINE__);
        return -1;
    }

    *size = 320;

    return 0;
}
#endif

G711AudioStreamSource *G711AudioStreamSource::createNew(UsageEnvironment& env) 
{
    G711AudioStreamSource* newSource = new G711AudioStreamSource(env);
    if (newSource != NULL && newSource->bitsPerSample() == 0) {
        // The WAV file header was apparently invalid.
        Medium::close(newSource);
        
        return NULL;
    }

    return newSource;
}

G711AudioStreamSource::G711AudioStreamSource(UsageEnvironment& env)
: FramedSource(env) 
, fNumChannels(0)
, fSamplingFrequency(0)
, fBitsPerSample(0)
, fLimitNumBytesToStream(False)
, fNumBytesToStream(0)
, fLastPlayTime(0)
, fPlayTimePerSample(0)
{
    fNumChannels = 1;
    fSamplingFrequency = 8000;
    fBitsPerSample = 8;

    fPlayTimePerSample = 1e6/(double)fSamplingFrequency;
    // Although PCM is a sample-based format, we group samples into
    // 'frames' for efficient delivery to clients.  Set up our preferred
    // frame size to be close to 20 ms, if possible, but always no greater
    // than 1400 bytes (to ensure that it will fit in a single RTP packet)
    unsigned maxSamplesPerFrame = (1400*8)/(fNumChannels*fBitsPerSample);
    unsigned desiredSamplesPerFrame = (unsigned)(0.04*fSamplingFrequency);
    unsigned samplesPerFrame = desiredSamplesPerFrame < maxSamplesPerFrame ? desiredSamplesPerFrame : maxSamplesPerFrame;
    fPreferredFrameSize = (samplesPerFrame*fNumChannels*fBitsPerSample)/8;

    audioOpen();
#if 0
    fp = NULL;
    fp = fopen("test.g711", "r");
    if(fp == NULL) {
        printf("ERR : %s : %d\n", __FILE__, __LINE__);
    }
#endif
}

G711AudioStreamSource::~G711AudioStreamSource()
{
    audioClose();
#if 0
    if(fp != NULL)
        fclose(fp);
#endif
}

// Note: We should change the following to use asynchronous file reading, #####
// as we now do with ByteStreamFileSource. #####
void G711AudioStreamSource::doGetNextFrame()
{
    // Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
    if (fLimitNumBytesToStream && fNumBytesToStream < fMaxSize) {
        fMaxSize = fNumBytesToStream;
    }

    if (fPreferredFrameSize < fMaxSize) {
        fMaxSize = fPreferredFrameSize;
    }

    unsigned bytesPerSample = (fNumChannels*fBitsPerSample)/8;
    if (bytesPerSample == 0) 
        bytesPerSample = 1; // because we can't read less than a byte at a time
    //unsigned bytesToRead = fMaxSize - fMaxSize%bytesPerSample;

    //fFrameSize : 1000
    audioGetOneFrame(fTo, &fFrameSize);

    // Set the 'presentation time' and 'duration' of this frame:
    if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
        // This is the first frame, so use the current time:
        gettimeofday(&fPresentationTime, NULL);
    } else {
        // Increment by the play time of the previous data:
        unsigned uSeconds	= fPresentationTime.tv_usec + fLastPlayTime;
        fPresentationTime.tv_sec += uSeconds/1000000;
        fPresentationTime.tv_usec = uSeconds%1000000;
    }

    // Remember the play time of this data:
    fDurationInMicroseconds = fLastPlayTime
        = (unsigned)((fPlayTimePerSample*fFrameSize)/bytesPerSample);

    // Switch to another task, and inform the reader that he has data:
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
            (TaskFunc*)FramedSource::afterGetting, this);

    return;
}


