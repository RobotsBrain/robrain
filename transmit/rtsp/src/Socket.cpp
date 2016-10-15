#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>


#include "base/Log.h"
#include "rtsp/Socket.h"

#define RTSP_EL "\r\n"
#define RTSP_VER "RTSP/1.0"
#define HDR_REQUIRE "Require"
#define HDR_ACCEPT "Accept"
#define PACKAGE "rtspd"
#define VERSION "1.0"
#define SDP_EL "\r\n"
#define HDR_TRANSPORT "Transport"
#define HDR_SESSION "Session"


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

typedef struct {
	const char *token;
	int code;
} Status;

Status status[] = {
		{"Continue", 100}, 
		{"OK", 200}, 
		{"Created", 201}, 
		{"Accepted", 202}, 
		{"Non-Authoritative Information", 203}, 
		{"No Content", 204}, 
		{"Reset Content", 205}, 
		{"Partial Content", 206}, 
		{"Multiple Choices", 300}, 
		{"Moved Permanently", 301}, 
		{"Moved Temporarily", 302}, 
		{"Bad Request", 400}, 
		{"Unauthorized", 401}, 
		{"Payment Required", 402}, 
		{"Forbidden", 403}, 
		{"Not Found", 404}, 
		{"Method Not Allowed", 405}, 
		{"Not Acceptable", 406}, 
		{"Proxy Authentication Required", 407}, 
		{"Request Time-out", 408}, 
		{"Conflict", 409}, 
		{"Gone", 410}, 
		{"Length Required", 411}, 
		{"Precondition Failed", 412}, 
		{"Request Entity Too Large", 413}, 
		{"Request-URI Too Large", 414}, 
		{"Unsupported Media Type", 415}, 
		{"Bad Extension", 420}, 
		{"Invalid Parameter", 450}, 
		{"Parameter Not Understood", 451}, 
		{"Conference Not Found", 452}, 
		{"Not Enough Bandwidth", 453}, 
		{"Session Not Found", 454}, 
		{"Method Not Valid In This State", 455}, 
		{"Header Field Not Valid for Resource", 456}, 
		{"Invalid Range", 457}, 
		{"Parameter Is Read-Only", 458}, 
		{"Unsupported transport", 461}, 
		{"Internal Server Error", 500}, 
		{"Not Implemented", 501}, 
		{"Bad Gateway", 502}, 
		{"Service Unavailable", 503}, 
		{"Gateway Time-out", 504}, 
		{"RTSP Version Not Supported", 505}, 
		{"Option not supported", 551}, 
		{"Extended Error:", 911}, 
		{NULL, -1} };

const char *get_stat(int err)
{
	int i;

	for (i = 0; status[i].code != err && status[i].code != -1; ++i);

	return status[i].token;
}

int send_reply(int clifd, int error, int cseq)
{
	char buf[1024] = "";
	 
	sprintf(buf, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL, RTSP_VER, error, 
		(char *)get_stat(error), cseq);

	strcat(buf, RTSP_EL);

	if(write(clifd, buf, strlen(buf))<0) {
	 	printf("send rtspd reply packet error\n");
		return -1;
 	}

	return 0;
}

int get_rtsp_cseg(const char *in, int &cseq)
{
	char *p = NULL;
	char trash[255];
	
	/****check  CSeq****/ 
	if ((p = strstr(in, "CSeq")) == NULL) {
		/**** not find CSeq send 400 error ****/
		return -1;
	}

	if(sscanf(p, "%254s %d", trash, &cseq) != 2) {
		/**** not find CSeq value send 400 error ****/
		return -1;
	}

	INFO("CSeq: %d\n", cseq);

	return 0;
}

int set_options_reply(int clifd, int status, int cseq)
{
	char out[RTSP_BUFFERSIZE] = {0};

	sprintf(out, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL, RTSP_VER, status, get_stat(status), cseq);
	strcat(out, "Public: OPTIONS,DESCRIBE,SETUP,PLAY,PAUSE,TEARDOWN"RTSP_EL);
	strcat(out, RTSP_EL);

	printf("%s\n", out);

	if(write(clifd, out, strlen(out)) < 0) {
		printf("set_options_reply error\n");
		return -1;
	}

	return 0;
}

int rtsp_options(int clifd, const char *in)
{
	int cseq = 0;

	if(get_rtsp_cseg(in, cseq) != -1) {
		set_options_reply(clifd, 200, cseq);
		return 1;
	} else {
		send_reply(clifd, 400, cseq);
	}

	return 0;
}

int rtsp_describe()
{
#if 0
	if(check_rtsp_url()<0) {
		return -1;
	}

	if(check_rtsp_filename()<0) {
		return -1;
	}

	// Disallow Header REQUIRE
	if (strstr(rtsp[cur_conn_num]->in_buffer, HDR_REQUIRE)) {
		send_reply(551);	/* Option not supported */
		return -1;
	}

	/* Get the description format. SDP is recomended */
	if (strstr(rtsp[cur_conn_num]->in_buffer, HDR_ACCEPT) != NULL) {
		if (strstr(rtsp[cur_conn_num]->in_buffer, "application/sdp") != NULL) {
			//descr_format = df_SDP_format;		
		} else {
			// Add here new description formats
			send_reply(551);	/* Option not supported */
			return -1;
		}
	}

	if(get_rtsp_cseg()<0) {
		return -1;
	}

	if(get_describe_sdp(rtsp[cur_conn_num]->sdp_buffer,cur_conn_num) < 0) {
		return -1;
	}

	if(send_describe_reply(200)!=-1) {
		return 1;
	}
#endif
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

  		RtspRead(clifd, in, sizeof(in));

  		printf("%s\n", in);

  		RTSPMETHOD method = ParseRtspCmdMethod(in);
  		switch(method) {
  		case RTSP_OPTIONS:
  			rtsp_options(clifd, in);
  			break;

  		case RTSP_DESCRIBE:

  			break;

		case RTSP_SETUP:
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


