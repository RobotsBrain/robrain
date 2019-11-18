#ifndef __RBAVHEADER_H__
#define __RBAVHEADER_H__

#include <sys/types.h>
#include <stdint.h>



enum AVTYPE {
	AV_AUDIO,
	AV_VIDEO,
};

typedef struct {
	u_char type;
	uint32_t timestamp;
	uint32_t size;
} RbAvHeader;


#endif