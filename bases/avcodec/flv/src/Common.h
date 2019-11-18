#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/types.h>
#include <stdint.h>



static const unsigned int nH264StartCode = 0x01000000;

static unsigned int ShowU32(u_char *pBuf) 
{
	return (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3];
}

static unsigned int ShowU24(u_char *pBuf) 
{
	return (pBuf[0] << 16) | (pBuf[1] << 8) | (pBuf[2]);
}

static unsigned int ShowU16(u_char *pBuf) 
{
	return (pBuf[0] << 8) | (pBuf[1]);
}

static unsigned int ShowU8(u_char *pBuf) 
{
	return (pBuf[0]);
}

static void WriteU64(uint64_t &x, int length, int value) 
{
	uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
	x = (x << length) | ((uint64_t) value & mask);
}

static unsigned int WriteU32(unsigned int n) 
{
	unsigned int nn = 0;
	u_char *p = (u_char *)&n;
	u_char *pp = (u_char *)&nn;
	
	pp[0] = p[3];
	pp[1] = p[2];
	pp[2] = p[1];
	pp[3] = p[0];

	return nn;
}

class u4 
{
public:
	u4(unsigned int i) 
	{
		_u[0] = i >> 24;
		_u[1] = (i >> 16) & 0xff;
		_u[2] = (i >> 8) & 0xff;
		_u[3] = i & 0xff;
  	} 

public:
	unsigned char _u[4];
};

class u3 
{
public:
	u3(unsigned int i) 
	{
		_u[0] = i >> 16;
		_u[1] = (i >> 8) & 0xff;
		_u[2] = i & 0xff;
  	} 

public:
	unsigned char _u[3];
};

class u2 
{
public:
	u2(unsigned int i) {
		_u[0] = i >> 8;
		_u[1] = i & 0xff;
  	} 

public:
	unsigned char _u[2];
};

#endif