#ifndef __VIDEOCAPTURE_H__
#define __VIDEOCAPTURE_H__

#include <pthread.h>
#include <linux/videodev2.h>
#include <string>

#include "DumpFile.h"
#include "X264Encoder.h"

#include "RTPSender.h"

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
	int Open(std::string devname, int width, int height);
	void InitMmap();
	void Close();
	void ReadAndEncodeFrame();

	static void *ThreadProc(void *argv);

private:
	pthread_t 				m_tid;
    pthread_attr_t 			m_attr;

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


