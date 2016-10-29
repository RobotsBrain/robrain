#ifndef _G711_AUDIO_STREAM_SOURCE_HH
#define _G711_AUDIO_STREAM_SOURCE_HH

#include "FramedSource.hh"



class G711AudioStreamSource: public FramedSource 
{
public:
    unsigned char bitsPerSample() const { return fBitsPerSample; }
  unsigned char numChannels() const { return fNumChannels; }
  unsigned samplingFrequency() const { return fSamplingFrequency; }

  static G711AudioStreamSource* createNew(UsageEnvironment& env);

protected:
  G711AudioStreamSource(UsageEnvironment& env);
	// called only by createNew()

  virtual ~G711AudioStreamSource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

private:
  unsigned char fNumChannels;
  unsigned fSamplingFrequency;
  unsigned char fBitsPerSample;
  unsigned fPreferredFrameSize;
  Boolean fLimitNumBytesToStream;
  unsigned fNumBytesToStream;
  unsigned fLastPlayTime;
  double fPlayTimePerSample; // useconds
};

#endif
