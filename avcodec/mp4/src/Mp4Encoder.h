#ifndef __MP4ENCODER_H__
#define __MP4ENCODER_H__

#include <sys/types.h>
#include <stdint.h>

#include "mp4v2/mp4v2.h"



typedef struct {
	int type;
	int size;
	u_char *data;
} NaluUnit;

typedef struct {
	// video, must be h264 type  
	uint32_t nSpsLen;
	u_char Sps[1024];
	uint32_t nPpsLen;
	u_char Pps[1024];
} Metadata, *PMetadata;



class Mp4Encoder {
public:
	Mp4Encoder();
	~Mp4Encoder();

	bool WriteH264File(const char *pFile264, const char *pFileMp4);

private:
	MP4FileHandle CreateMP4File(const char *fileName, int width, int height,
								 int timeScale = 90000, int frameRate = 25);

	void CloseMP4File(MP4FileHandle hMp4File);

	int WriteH264Data(MP4FileHandle hMp4File, const u_char *pData, int size);

	int ReadOneNalu(const u_char *buffer, uint32_t size, uint32_t offSet, NaluUnit &nalu);

private:
	int m_nWidth;
	int m_nHeight;
	int m_nFrameRate;
	int m_nTimeScale;
	MP4TrackId m_videoId;
};

#endif