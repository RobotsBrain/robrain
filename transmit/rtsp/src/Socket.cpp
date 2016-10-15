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
#include "rtsp/Socket.h"


using namespace std;

namespace Rtsp {

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



CSocket::CSocket()
: Base::CThreadLoop("RtspServer")
{
}

CSocket::~CSocket()
{
}

bool CSocket::Start(const char *host, int port)
{
    int reuse = 1;
    struct sockaddr_in server;

	if(host == NULL) {
	    return false;
	}

	m_rtspfd = socket(AF_INET, SOCK_STREAM, 0);
	if(m_rtspfd < 0){
    	ERROR("socket error!\n");
    	return false;
	}

	setsockopt(m_rtspfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	memset(&server, 0, sizeof(struct sockaddr_in));

  	server.sin_family = AF_INET;
  	server.sin_addr.s_addr = inet_addr(host);
  	server.sin_port = htons(port);

  	bind(m_rtspfd, (struct sockaddr *)&server, sizeof(struct sockaddr));

  	listen(m_rtspfd, 5);

  	DEBUG("rtsp server listen sucess\n");

	return StartThread();
}

bool CSocket::Stop()
{
	close(m_rtspfd);

	return StopThread();
}

#define RTSP_BUFFERSIZE 4096

enum RTSPMETHOD {
	RTSP_OPTIONS,
	RTSP_DESCRIBE,
	RTSP_SETUP,
	RTSP_PLAY,
	RTSP_TEARDOWN,
};

string RtspMethodString[] = {"OPTIONS", "DESCRIBE", "SETUP", "PLAY", "TEARDOWN"};

RTSPMETHOD ParseRtspCmdMethod(const char *buf)
{
	int i = 0;

	for(int i = 0; i < sizeof(RtspMethodString); i++) {
		if(strncmp(buf, RtspMethodString[i].c_str(), RtspMethodString[i].size()) == 0) {
			return (RTSPMETHOD)i;
		}
	}
}

int send_reply(int clifd, int error, int cseq)
{
	char buf[1024] = "";
	 
	// sprintf(buf, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL, RTSP_VER, error, 
	// 	(char *)get_stat(error), cseq);

	// strcat(buf, RTSP_EL);

	if(write(clifd, buf, strlen(buf))<0) {
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
		send_reply(clifd, err, cseq);
		return -1;
	}

	if(write(clifd, response.c_str(), response.size())<0){
		printf("send_describe_reply error\n");
		return -1;
	}

	printf("%s\n", response.c_str());

	return 0;
}

void CSocket::EventHandleLoop()
{
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    int clifd;

    char in[RTSP_BUFFERSIZE];

	while(1) {

  		clifd = accept(m_rtspfd, (struct sockaddr *)&client, &len);

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
				break;

			case RTSP_PLAY:
				break;

			case RTSP_TEARDOWN:
				break;
	  		}
  		}
  		
		if(WaitForSleep(10000) < 0) {
			break;
		}
	}

	return;
}

} // end namespace


