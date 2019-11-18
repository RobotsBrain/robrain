#ifndef __FLV_H__
#define __FLV_H__

#include <sys/types.h>
#include <stdint.h>



namespace Flv {

#define FLV_HEADER_SIZE 	9

typedef struct {
	int nVersion;
	int bHaveVideo;
	int bHaveAudio;
	int nHeadSize;
} FlvHeader;

typedef struct {
	uint8_t 	nType;
	uint32_t 	nDataSize;
	uint32_t 	nTimeStamp;
	uint8_t 	nTSEx;
	uint32_t 	nStreamID;

	uint32_t 	nTotalTS;
} TagHeader;

typedef struct {
	int nFrameType;
	int nCodecID;
	int nAVCPacketType;
	int nCompositionTime;
} VideoTag;

typedef struct {
	int nSoundFormat;
	int nSoundRate;
	int nSoundSize;
	int nSoundType;
	int nAACPacketType;
} AudioTag;

} // end namespace


#endif