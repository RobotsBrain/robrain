#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "X264Encoder.h"



CX264Encoder::CX264Encoder()
: m_param(NULL)
, m_handle(NULL)
, m_picture(NULL)
, m_nal(NULL)
{

}

CX264Encoder::~CX264Encoder()
{

}

void CX264Encoder::Start(int width, int height)
{
	m_param = (x264_param_t *)malloc(sizeof(x264_param_t));
	m_picture = (x264_picture_t *)malloc(sizeof(x264_picture_t));

	x264_param_default(m_param);	//set default param
	//en->param->rc.i_rc_method = X264_RC_CQP;//设置为恒定码率
	//en->param->i_log_level = X264_LOG_NONE;
	//en->param->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;//取空缓存区使用不死锁保证
	//en->param->i_frame_total = 0;
	//en->param->i_keyint_max = 10; 
	//en->param->i_bframe = 5; //两个参考帧之间b帧的数目
	//en->param->b_open_gop = 0;
	//en->param->i_bframe_pyramid = 0;
	//en->param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
	//en->param->rc.i_bitrate = 1024 * 10;//rate 为10 kbps
	m_param->i_width = width;	//set frame width
	m_param->i_height = height;	//set frame height
	m_param->rc.i_lookahead = 0;	//表示i帧向前缓冲区
	m_param->i_fps_num = 5;	//帧率分子
	m_param->i_fps_den = 1;	//帧率分母
	m_param->i_csp = X264_CSP_I422;
	x264_param_apply_profile(m_param, x264_profile_names[0]);	//使用baseline

	if ((m_handle = x264_encoder_open(m_param)) == 0) {
		printf("x264_encoder_open error!\n");
		return;
	}

	/* Create a new pic */
	x264_picture_alloc(m_picture, X264_CSP_I422, m_param->i_width, m_param->i_height);
	m_picture->img.i_csp = X264_CSP_I422;
	m_picture->img.i_plane = 3;

	return;
}

void CX264Encoder::Stop()
{
	if(m_picture) {
		x264_picture_clean(m_picture);
		free(m_picture);
		m_picture = NULL;
	}

	if(m_param) {
		free(m_param);
		m_param = NULL;
	}

	if (m_handle) {
		x264_encoder_close(m_handle);
		m_handle = NULL;
	}

	return;
}

int CX264Encoder::CompressFrame(int type, uint8_t *in, uint8_t *out)
{
	x264_picture_t pic_out;
	int nNal = -1;
	int result = 0;
	int i = 0;
	uint8_t *p_out = out;

	uint8_t *y = m_picture->img.plane[0];
	uint8_t *u = m_picture->img.plane[1];
	uint8_t *v = m_picture->img.plane[2];

	int is_y = 1, is_u = 1;
	int y_index = 0, u_index = 0, v_index = 0;
	int yuv422_length = 2 * m_param->i_width * m_param->i_height;

	//序列为YU YV YU YV，一个yuv422帧的长度 width * height * 2 个字节
	for (i = 0; i < yuv422_length; ++i) {
		if (is_y) {
			*(y + y_index) = *(in + i);
			++y_index;
			is_y = 0;
		} else {
			if (is_u) {
				*(u + u_index) = *(in + i);
				++u_index;
				is_u = 0;
			} else {
				*(v + v_index) = *(in + i);
				++v_index;
				is_u = 1;
			}

			is_y = 1;
		}
	}

	switch (type) {
	case 0:
		m_picture->i_type = X264_TYPE_P;
		break;

	case 1:
		m_picture->i_type = X264_TYPE_IDR;
		break;

	case 2:
		m_picture->i_type = X264_TYPE_I;
		break;

	default:
		m_picture->i_type = X264_TYPE_AUTO;
		break;
	}

	//printf("en->handle->param.i_csp:%08x\n", en->handle->param.i_csp);
	if (x264_encoder_encode(m_handle, &(m_nal), &nNal, m_picture, &pic_out) < 0) {
		printf("x264 encode error, type: %08x!\n", m_picture->img.i_csp);
		return -1;
	}

	for (i = 0; i < nNal; i++) {
		memcpy(p_out, m_nal[i].p_payload, m_nal[i].i_payload);
		p_out += m_nal[i].i_payload;
		result += m_nal[i].i_payload;
	}

	return result;
}


