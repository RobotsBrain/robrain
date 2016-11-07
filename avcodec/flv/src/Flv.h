#ifndef __FLV_H__
#define __FLV_H__

#include <sys/types.h>
#include <stdint.h>


typedef struct {
	int nVersion;
	int bHaveVideo;
	int bHaveAudio;
	int nHeadSize;

	u_char *pFlvHeader;
} FlvHeader;

class CFlv
{
public:
	CFlv();
	~CFlv();
	
};

#endif