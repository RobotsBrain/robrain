#ifndef __MP4ENCODER_H__
#define __MP4ENCODER_H__

#include "mp4v2/mp4v2.h"



typedef struct {
	int type;
	int size;
	unsigned char *data;
} NaluUnit;

typedef struct {
	// video, must be h264 type  
	unsigned int nSpsLen;
	unsigned char Sps[1024];
	unsigned int nPpsLen;
	unsigned char Pps[1024];
} Metadata, *PMetadata;



class Mp4Encoder {
public:
	Mp4Encoder();
	~Mp4Encoder();

public:
	MP4FileHandle CreateMP4File(const char *fileName, int width, int height,
								 int timeScale = 90000, int frameRate = 25);

	bool Write264Metadata(MP4FileHandle hMp4File, PMetadata lpMetadata);

	int WriteH264Data(MP4FileHandle hMp4File, const unsigned char *pData, int size);

	void CloseMP4File(MP4FileHandle hMp4File);

	bool WriteH264File(const char *pFile264, const char *pFileMp4);

	bool PraseMetadata(const unsigned char *pData, int size, Metadata &metadata);

private:
	int ReadOneNaluFromBuf(const unsigned char *buffer, unsigned int nBufferSize, 
                            unsigned int offSet, NaluUnit &nalu);

private:
	int m_nWidth;
	int m_nHeight;
	int m_nFrameRate;
	int m_nTimeScale;
	MP4TrackId m_videoId;
};

#endif