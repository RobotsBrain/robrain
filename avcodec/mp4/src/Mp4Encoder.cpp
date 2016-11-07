#include <string.h>

#include "Mp4Encoder.h"



#define BUFFER_SIZE  (1024*1024)

Mp4Encoder::Mp4Encoder()
: m_videoId(NULL)
, m_nWidth(0)
, m_nHeight(0)
, m_nTimeScale(0)
, m_nFrameRate(0)
{
}

Mp4Encoder::~Mp4Encoder()
{
}

MP4FileHandle Mp4Encoder::CreateMP4File(const char *pFileName, int width, int height,
										int timeScale /* = 90000 */, int frameRate /* = 25 */)
{
	if (pFileName == NULL) {
		return false;
	}

	MP4FileHandle hMp4file = MP4Create(pFileName);
	if (hMp4file == MP4_INVALID_FILE_HANDLE) {
		printf("ERROR:Open file fialed.\n");
		return false;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nTimeScale = 90000;
	m_nFrameRate = 25;

	MP4SetTimeScale(hMp4file, m_nTimeScale);

	return hMp4file;
}

void Mp4Encoder::CloseMP4File(MP4FileHandle hMp4File)
{
	if (hMp4File) {
		MP4Close(hMp4File);
		hMp4File = NULL;
	}
}

int Mp4Encoder::ReadOneNalu(const u_char *buffer, uint32_t size, uint32_t offSet, NaluUnit &nalu)
{
	int i = offSet;

	while (i < size) {
		if (buffer[i++] == 0x00 && buffer[i++] == 0x00 
            && buffer[i++] == 0x00 && buffer[i++] == 0x01) {

			int pos = i;

			while (pos < size) {
				if (buffer[pos++] == 0x00 && buffer[pos++] == 0x00 
					&& buffer[pos++] == 0x00 && buffer[pos++] == 0x01) {
					break;
				}
			}

			if (pos == size) {
				nalu.size = pos - i;
			} else {
				nalu.size = (pos - 4) - i;
			}

			nalu.type = buffer[i] & 0x1f;
			nalu.data = (u_char *)&buffer[i];

			return (nalu.size + i - offSet);
		}
	}

	return 0;
}

int Mp4Encoder::WriteH264Data(MP4FileHandle hMp4File, const u_char *pData, int size)
{
	NaluUnit nalu;
	int pos = 0, len = 0;

	while (len = ReadOneNalu(pData, size, pos, nalu)) {
		if (nalu.type == 0x07) {	// sps  
		
			// 添加h264 track
			if(m_videoId == NULL) {
				m_videoId = MP4AddH264VideoTrack(hMp4File, 
											 m_nTimeScale, 
											 m_nTimeScale/m_nFrameRate, 
											 m_nWidth,	// width  
											 m_nHeight,	// height  
											 nalu.data[1],	// sps[1] AVCProfileIndication  
											 nalu.data[2],	// sps[2] profile_compat  
											 nalu.data[3],	// sps[3] AVCLevelIndication  
											 3);	// 4 bytes length before each NAL unit  
				if (m_videoId == MP4_INVALID_TRACK_ID) {
					printf("add video track failed.\n");
					return 0;
				}

				MP4SetVideoProfileLevel(hMp4File, 0x7F);	//  Simple Profile @ Level
			}
			
			MP4AddH264SequenceParameterSet(hMp4File, m_videoId, nalu.data, nalu.size);
		} else if (nalu.type == 0x08) {	// pps		
			MP4AddH264PictureParameterSet(hMp4File, m_videoId, nalu.data, nalu.size);
		} else {
			int datalen = nalu.size + 4;
			u_char *data = new u_char[datalen];

			// MP4 Nalu前四个字节表示Nalu长度  
			data[0] = nalu.size >> 24;
			data[1] = nalu.size >> 16;
			data[2] = nalu.size >> 8;
			data[3] = nalu.size & 0xff;

			memcpy(data + 4, nalu.data, nalu.size);

			if (!MP4WriteSample(hMp4File, m_videoId, data, datalen, MP4_INVALID_DURATION, 0, 1)) {
				return 0;
			}

			delete[] data;
		}

		pos += len;
	}

	return pos;
}

bool Mp4Encoder::WriteH264File(const char *pFile264, const char *pFileMp4)
{
	if (pFile264 == NULL || pFileMp4 == NULL) {
		return false;
	}

	MP4FileHandle hMp4File = CreateMP4File(pFileMp4, 320, 240);
	if (hMp4File == NULL) {
		printf("ERROR:Create file failed!");
		return false;
	}

	FILE *fp = fopen(pFile264, "rb");
	if (!fp) {
		printf("ERROR:open file failed!");
		return false;
	}

	fseek(fp, 0, SEEK_SET);

	u_char *buffer = new u_char[BUFFER_SIZE];
	int pos = 0;

	while (1) {
		int readlen = fread(buffer + pos, sizeof(u_char), BUFFER_SIZE - pos, fp);
		if (readlen <= 0) {
			break;
		}

		readlen += pos;

		int writelen = 0;

		for (int i = readlen - 1; i >= 0; i--) {
			if (buffer[i--] == 0x01 && buffer[i--] == 0x00 
				&& buffer[i--] == 0x00 && buffer[i--] == 0x00) {
				writelen = i + 5;
				break;
			}
		}

		writelen = WriteH264Data(hMp4File, buffer, writelen);
		if (writelen <= 0) {
			break;
		}

		memcpy(buffer, buffer + writelen, readlen - writelen + 1);
		pos = readlen - writelen + 1;
	}

	fclose(fp);

	delete[]buffer;
	CloseMP4File(hMp4File);

	return true;
}


#if 0
//添加aac音频
MP4TrackId audio = MP4AddAudioTrack(file, 48000, 1024, MP4_MPEG4_AUDIO_TYPE);
if (video == MP4_INVALID_TRACK_ID) {
	printf("add audio track failed.\n");
	return;
}

MP4SetAudioProfileLevel(file, 0x2);

MP4WriteSample(file, audio, pf->p_frame, pf->i_frame_size , MP4_INVALID_DURATION, 0, 1);

#endif