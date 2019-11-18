#include <string.h>

#include "Mp4Encoder.h"

Mp4Encoder::Mp4Encoder()
:	m_videoId(MP4_INVALID_TRACK_ID)
	, m_audioId(MP4_INVALID_TRACK_ID)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nVTimeScale(0)
	, m_nFrameRate(0)
	, m_hMp4File(NULL)
{
}

Mp4Encoder::~Mp4Encoder()
{
}

void Mp4Encoder::Start(const char *filename, int width, int height,
					   int vtimescale, int framerate, int atimescale)
{
	if (filename == NULL)
		return;

	m_hMp4File = MP4Create(filename);
	if (m_hMp4File == MP4_INVALID_FILE_HANDLE) {
		printf("ERROR:Open file fialed.\n");
		return;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nVTimeScale = 90000;
	m_nFrameRate = 25;
	m_nATimeScale = 48000;

	return;
}

void Mp4Encoder::Stop()
{
	if (m_hMp4File) {
		MP4Close(m_hMp4File);
		m_hMp4File = NULL;
	}
}

int Mp4Encoder::WriteH264Data(u_char * pData, int size)
{
	int type = pData[4] & 0x1f;

	if (type == 0x07) {			// sps

		// 添加h264 track
		if (m_videoId == MP4_INVALID_TRACK_ID) {
			m_videoId = MP4AddH264VideoTrack(m_hMp4File, m_nVTimeScale, m_nVTimeScale / m_nFrameRate, m_nWidth,	// width
											 m_nHeight,	// height
											 pData[5],	// sps[1] AVCProfileIndication
											 pData[6],	// sps[2] profile_compat
											 pData[7],	// sps[3] AVCLevelIndication
											 3);	// 4 bytes length before each NAL unit
			if (m_videoId == MP4_INVALID_TRACK_ID) {
				printf("add video track failed.\n");
				return 0;
			}

			MP4SetVideoProfileLevel(m_hMp4File, 0x7F);	//  Simple Profile @ Level
		}

		MP4AddH264SequenceParameterSet(m_hMp4File, m_videoId, pData + 4,
									   size - 4);
	} else if (type == 0x08) {	// pps
		MP4AddH264PictureParameterSet(m_hMp4File, m_videoId, pData + 4,
									  size - 4);
	} else {
		// MP4 Nalu前四个字节表示Nalu长度(no 00 00 00 01)
		pData[0] = (size - 4) >> 24;
		pData[1] = (size - 4) >> 16;
		pData[2] = (size - 4) >> 8;
		pData[3] = (size - 4) & 0xff;

		if (!MP4WriteSample
			(m_hMp4File, m_videoId, pData, size, MP4_INVALID_DURATION, 0, 1)) {
			return 0;
		}
	}

	return 0;
}

int Mp4Encoder::WriteAacData(u_char * pData, int size)
{
	if (m_audioId == MP4_INVALID_TRACK_ID) {
		m_audioId =
			MP4AddAudioTrack(m_hMp4File, 48000, 1024, MP4_MPEG4_AUDIO_TYPE);
		if (m_audioId == MP4_INVALID_TRACK_ID) {
			printf("add audio track failed.\n");
			return 0;
		}

		MP4SetAudioProfileLevel(m_hMp4File, 0x2);
	}

	MP4WriteSample(m_hMp4File, m_audioId, pData, size, MP4_INVALID_DURATION, 0,
				   1);

	return 0;
}
