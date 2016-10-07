#ifndef __AVPACKET_H__
#define __AVPACKET_H__

enum AVTYPE {
	AV_TYPE_AUDIO,
	AV_TYPE_VIDEO	
};

typedef struct AvPacket {
	AVTYPE type;
	long long pts;
	long len;
} AvPacket;

#endif


