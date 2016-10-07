#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__

#include <linux/videodev2.h>
#include <string>

#include "DumpFile.h"
#include "X264Encoder.h"
#include "ThreadLoop.h"
#include "RTPSender.h"



struct Buffer {
	void 	*start;
	size_t 	length;
};

class CVideoCapture: public Base::CThreadLoop
{
public:
	CVideoCapture();
	~CVideoCapture();

	bool Start();
	bool Stop();

private:
	int Open(std::string devname, int width, int height);
	void InitMmap();
	void Close();
	
	virtual void EventHandleLoop();

private:
	int						m_vfd;
	int 					m_buff_num;
	struct Buffer 			*m_buffers;

	struct v4l2_capability 	m_v4l2_cap;
	struct v4l2_cropcap 	m_v4l2_cropcap;
	struct v4l2_format 		m_v4l2_fmt;
	struct v4l2_crop 		m_v4l2_crop;

#ifdef BUILD_DUMP_H264FILE
	Base::CDumpFile			m_h264file;
#endif
	
	CX264Encoder			m_x264encoder;

	CRTPSender				m_rtpsender;
};

#endif


