#ifndef __X264ENCODER_H__
#define __X264ENCODER_H__

#include <stdint.h>
#include <stdio.h>

#include "x264.h"



class CX264Encoder
{
public:
	CX264Encoder();
	~CX264Encoder();

public:
	void Start(int width, int height);
	void Stop();
	int CompressFrame(int type, uint8_t *in, uint8_t *out);

private:
	x264_param_t 	*m_param;
	x264_t 			*m_handle;
	x264_picture_t 	*m_picture;	//说明一个视频序列中每帧特点
	x264_nal_t 		*m_nal;	
};

#endif