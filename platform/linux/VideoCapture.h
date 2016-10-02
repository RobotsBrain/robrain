#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__


#include <linux/videodev2.h>



struct Buffer {
	void 	*start;
	size_t 	length;
};

class CVideoCapture
{
public:
	CVideoCapture();
	~CVideoCapture();

	bool Start();
	bool Stop();

private:
	int						m_vfd;
	int 					m_buff_num;
	struct Buffer 			*m_buffers;

	struct v4l2_capability 	m_v4l2_cap;
	struct v4l2_cropcap 	m_v4l2_cropcap;
	struct v4l2_format 		m_v4l2_fmt;
	struct v4l2_crop 		m_v4l2_crop;
};

#endif