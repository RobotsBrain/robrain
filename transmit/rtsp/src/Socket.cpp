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

#include "RtspClientSession.h"
#include "rtsp/Socket.h"



namespace Rtsp {


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

void CSocket::EventHandleLoop()
{
	int clifd = 0;
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);
    CRtspClientSession clientses;
    
	while(1) {

  		clifd = accept(m_rtspfd, (struct sockaddr *)&client, &len);
  		
  		clientses.Start(clifd);

		if(WaitForSleep(10000) < 0) {
			break;
		}
	}

	return;
}

} // end namespace


