#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>				/* low-level i/o */
#include <asm/types.h>			/* for videodev2.h */
#include <linux/videodev2.h>

#include "AvPacket.h"
#include "AvFrameRingBuffer.h"
#include "VideoCapture.h"


static inline int camera_ioctl(int fd, int request, void *arg)
{
	int r = -1;

	do {
		r = ioctl(fd, request, arg);
	} while (r < 0 && EINTR == errno);

	return r;
}

/*****************************************************************************************/

CVideoCapture::CVideoCapture()
: Base::CThreadLoop("VideoCapture")
, m_vfd(-1)
, m_buff_num(0)
, m_buffers(NULL)
{

}

CVideoCapture::~CVideoCapture()
{

}

bool CVideoCapture::Start()
{
    m_rtpsender.Start();

	return StartThread();
}

bool CVideoCapture::Stop()
{
	m_rtpsender.Stop();

	return StopThread();
}

int CVideoCapture::Open(std::string devname, int width, int height)
{
	m_vfd = open(devname.c_str(), O_RDWR, 0);	//  | O_NONBLOCK
	if (m_vfd < 0) {
		printf("Cannot open '%s': %d, %s\n", devname.c_str(), errno, strerror(errno));
		return -1;
	}

	unsigned int min;
	struct v4l2_capability *cap = &(m_v4l2_cap);
	struct v4l2_cropcap *cropcap = &(m_v4l2_cropcap);
	struct v4l2_crop *crop = &(m_v4l2_crop);
	struct v4l2_format *fmt = &(m_v4l2_fmt);

	if (camera_ioctl(m_vfd, VIDIOC_QUERYCAP, cap) < 0) {
		if (EINVAL == errno) {
			fprintf(stderr, "no V4L2 device\n");
			return -1;
		} else {
			printf("VIDIOC_QUERYCAP\n");
			return -1;
		}
	}

	if (!(cap->capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "no video capture device\n");
		return -1;
	}

	if (!(cap->capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "not support streaming i/o\n");
		return -1;
	}

	printf("\nVIDOOC_QUERYCAP\n");
	printf("driver: %s, card: %s, bus info: %s, version: %d\n", 
			cap->driver, cap->card, cap->bus_info, cap->version);

	/* Select video input, video standard and tune here. */
	cropcap->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	crop->c.width = width;
	crop->c.height = height;
	crop->c.left = 0;
	crop->c.top = 0;
	crop->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt->fmt.pix.width = width;
	fmt->fmt.pix.height = height;
	fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;	//yuv422
	//fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420  //yuv420
	fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;	//隔行扫描

	if (camera_ioctl(m_vfd, VIDIOC_S_FMT, fmt) < 0) {
		printf("VIDIOC_S_FMT\n");
		return -1;
	}

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt->fmt.pix.width * 2;
	if (fmt->fmt.pix.bytesperline < min)
		fmt->fmt.pix.bytesperline = min;
	min = fmt->fmt.pix.bytesperline * fmt->fmt.pix.height;
	if (fmt->fmt.pix.sizeimage < min)
		fmt->fmt.pix.sizeimage = min;

	return 0;
}

void CVideoCapture::InitMmap()
{
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	memset(&req, 0, sizeof(struct v4l2_requestbuffers));
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	//分配内存
	if (camera_ioctl(m_vfd, VIDIOC_REQBUFS, &req) < 0) {
		if (EINVAL == errno) {
			printf("not support memory mapping\n");
			return;
		} else {
			printf("VIDIOC_REQBUFS\n");
			return;
		}
	}

	if (req.count < 2) {
		printf("Insufficient buffer memory\n");
		return;
	}

	m_buff_num = req.count;

	m_buffers = (struct Buffer *)calloc(req.count, sizeof(struct Buffer));
	if (!m_buffers) {
		printf("Out of memory\n");
		return;
	}

	for (int cnt = 0; cnt < req.count; cnt++) {

		buf.index = cnt;

		//将VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
		if (camera_ioctl(m_vfd, VIDIOC_QUERYBUF, &buf) < 0) {
			printf("VIDIOC_QUERYBUF\n");
			return;
		}

		m_buffers[cnt].length = buf.length;
		m_buffers[cnt].start = mmap(NULL /* start anywhere */ ,
									buf.length,
									PROT_READ | PROT_WRITE
									/* required */ ,
									MAP_SHARED /* recommended */ ,
									m_vfd, buf.m.offset);

		if (MAP_FAILED == m_buffers[cnt].start) {
			printf("mmap\n");
			return;
		}
	}

	return;
}

void CVideoCapture::Close()
{
	uint32_t i;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (camera_ioctl(m_vfd, VIDIOC_STREAMOFF, &type) < 0) {
		printf("VIDIOC_STREAMOFF\n");
		return;
	}

	for (i = 0; i < m_buff_num; ++i) {
		if (munmap(m_buffers[i].start, m_buffers[i].length) < 0) {
			printf("munmap\n");
			return;
		}
	}

	free(m_buffers);

	close(m_vfd);

	return;
}

void CVideoCapture::EventHandleLoop()
{
	uint32_t i;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	Open("/dev/video0", 640, 480);
	InitMmap();

	uint8_t *h264_buf = (uint8_t *) malloc(sizeof(uint8_t) * 640 * 480 * 3);

	m_x264encoder.Start(640, 480);

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	for (i = 0; i < m_buff_num; ++i) {
		buf.index = i;

		if (camera_ioctl(m_vfd, VIDIOC_QBUF, &buf) < 0) {
			printf("VIDIOC_DQBUF\n");
		}
	}

	if (camera_ioctl(m_vfd, VIDIOC_STREAMON, &type) < 0) {
		printf("VIDIOC_STREAMON\n");
	}

	while(1) {
		memset(&buf, 0, sizeof(struct v4l2_buffer));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		//this operator below will change buf.index(0 <= buf.index <= 3)
		if (camera_ioctl(m_vfd, VIDIOC_DQBUF, &buf) < 0) {
			switch (errno) {
			case EAGAIN:
				break;
			case EIO:
				/* Could ignore EIO, see spec. */
				/* fall through */
			default:
				printf("VIDIOC_DQBUF\n");
				break;
			}
		}

		uint8_t *yuv_frame = (uint8_t *)m_buffers[buf.index].start;

		if (yuv_frame[0] != '\0') {
			int h264_length = m_x264encoder.CompressFrame(-1, yuv_frame, h264_buf);
			// m_rtpsender.SendH264Nalu(h264_buf, h264_length);
			if(h264_length > 0) {
				AvPacket packet;

				packet.type = AV_TYPE_VIDEO;
				packet.len = h264_length;

				CAvFrameRingBuffer::Instance()->WriteFrame(packet, (char *)h264_buf, h264_length);
			}
		}

		if (camera_ioctl(m_vfd, VIDIOC_QBUF, &buf) < 0) {
			printf("VIDIOC_DQBUF\n");
			break;
		}

		if(WaitForSleep(10) < 0) {
			break;
		}
	}
	
	free(h264_buf);

	m_x264encoder.Stop();
	
	Close();

	return;
}


