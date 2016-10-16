#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "base/Log.h"

#include "Rtsp.h"
#include "RtspClientSession.h"


using namespace std;

namespace Rtsp {

#define RTSP_BUFFERSIZE 4096

enum RTSPMETHOD {
	RTSP_OPTIONS,
	RTSP_DESCRIBE,
	RTSP_SETUP,
	RTSP_PLAY,
	RTSP_TEARDOWN,
};

string RtspMethodString[] = {"OPTIONS", "DESCRIBE", "SETUP", "PLAY", "TEARDOWN"};

ssize_t RtspRead(int fd, void *buf, int len)
{
    ssize_t size = 0;
	ssize_t left = len;
	void *ptr = buf;

	while(1) {
	    size = read(fd, ptr, left);
		if(size < 0) {
		    if(EINTR == errno) {
				continue;
		    } else {
				return -1;
		    }
		} else {
			break;
		}
	}

	return size;
}

ssize_t RtspWrite(int fd, void *buf, int len)
{
    ssize_t size;
	ssize_t left = len;
	char *ptr = (char *)buf;

	while(left > 0) {
	    size = write(fd, ptr, left);
		if(size < 0) {
		    if(EINTR == errno) {
				continue;
		    } else {
				return -1;
			}
		}

		left -= size;
		ptr += size;
	}

	return (len - left);
}

RTSPMETHOD ParseRtspCmdMethod(const char *buf)
{
	int i = 0;

	for(int i = 0; i < sizeof(RtspMethodString); i++) {
		if(strncmp(buf, RtspMethodString[i].c_str(), RtspMethodString[i].size()) == 0) {
			return (RTSPMETHOD)i;
		}
	}
}

int SendReply(int clifd, int error, int cseq)
{
	string response;

	GetResponse(error, cseq, response);

	if(write(clifd, response.c_str(), response.size())<0) {
	 	printf("send rtspd reply packet error\n");
		return -1;
 	}

	return 0;
}

int RtspOptions(int clifd, const char *in)
{
	int cseq = 0;

	if(GetRtspCseg(in, cseq) < 0) {
		return -1;
	}

	string response;

	SetOptionsReply(200, cseq, response);
	if(write(clifd, response.c_str(), response.size()) < 0) {
		printf("set_options_reply error\n");
		return -1;
	}
	
	printf("%s\n", response.c_str());

	return 0;
}

int RtspDescribe(int clifd, const char *in)
{
	int err = 0;
	string response;
	int cseq = 0;

	if(GetRtspCseg(in, cseq) < 0) {
		return -1;
	}

	if(SetDescribeReply(in, cseq, err, response) < 0) {
		SendReply(clifd, err, cseq);
		return -1;
	}

	if(write(clifd, response.c_str(), response.size()) < 0) {
		printf("send_describe_reply error\n");
		return -1;
	}

	printf("%s\n", response.c_str());

	return 0;
}

int RtspSetup(int clifd, const char *in)
{
	int err = 0;
	int cseq = 0;
	string response;

	if(GetRtspCseg(in, cseq) < 0) {
		return -1;
	}

	if(SetSetupReply(in, cseq, err, response) < 0) {
		SendReply(clifd, err, cseq);
		return -1;
	}

	if(write(clifd, response.c_str(), response.size()) < 0) {
		printf("send_describe_reply error\n");
		return -1;
	}

	printf("%s\n", response.c_str());

	return 0;
}

CRtspClientSession::CRtspClientSession()
: Base::CThreadLoop("RtspClientSession")
{
}

CRtspClientSession::~CRtspClientSession()
{
}

bool CRtspClientSession::Start(int fd)
{
	m_clifd = fd;

	return StartThread();
}

bool CRtspClientSession::Stop()
{
	close(m_clifd);

	return StopThread();
}

void CRtspClientSession::EventHandleLoop()
{
	int clifd = m_clifd;
	char in[RTSP_BUFFERSIZE];

	while(1) {

		RtspRead(clifd, in, sizeof(in));

  		printf("%s\n", in);

  		RTSPMETHOD method = ParseRtspCmdMethod(in);
  		switch(method) {
  		case RTSP_OPTIONS:
  			RtspOptions(clifd, in);
  			break;

  		case RTSP_DESCRIBE:
  			RtspDescribe(clifd, in);
  			break;

		case RTSP_SETUP:
			RtspSetup(clifd, in);
			break;

		case RTSP_PLAY:
			break;

		case RTSP_TEARDOWN:
			break;
  		}

		if(WaitForSleep(10000) < 0) {
			break;
		}
	}

	return;
}

} // end namespace


