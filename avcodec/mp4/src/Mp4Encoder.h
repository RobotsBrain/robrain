#ifndef __MP4ENCODER_H__
#define __MP4ENCODER_H__

#include <sys/types.h>
#include <stdint.h>

#include "mp4v2/mp4v2.h"



class Mp4Encoder {
public:
	Mp4Encoder();
	~Mp4Encoder();

	void Start(const char *filename, int width, int height, 
				int vtimescale = 90000, int framerate = 25, int atimescale = 48000);
	void Stop();

	int WriteH264Data(u_char *pData, int size);
	int WriteAacData(u_char *pData, int size);

private:
	int m_nWidth;
	int m_nHeight;
	int m_nFrameRate;
	int m_nVTimeScale;
	int m_nATimeScale;

	MP4TrackId m_audioId;
	MP4TrackId m_videoId;
	MP4FileHandle m_hMp4File;
};

#endif