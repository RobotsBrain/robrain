#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "FetchRtmpStream.h"

using namespace std;



namespace Rtmp {

int CreateRtmp(string url, RTMP **pprtmp)
{
	bool bLiveStream = true;
	RTMP *rtmp = RTMP_Alloc();
	RTMP_Init(rtmp);
	
	rtmp->Link.timeout = 10; //set connection timeout,default 30s

	if(!RTMP_SetupURL(rtmp, (char *)url.c_str())) {
		RTMP_Log(RTMP_LOGERROR, "SetupURL Err\n");
		RTMP_Free(rtmp);
		return -1;
	}

	if (bLiveStream) {
		rtmp->Link.lFlags |= RTMP_LF_LIVE;
	}

	RTMP_SetBufferMS(rtmp, 3600*1000); // 1hour

	if(!RTMP_Connect(rtmp, NULL)) {
		RTMP_Log(RTMP_LOGERROR, "Connect Err\n");
		RTMP_Free(rtmp);
		return -1;
	}

	if(!RTMP_ConnectStream(rtmp, 0)) {
		RTMP_Log(RTMP_LOGERROR, "ConnectStream Err\n");
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		return -1;
	}

	*pprtmp = rtmp;

	return 0;
}

int DestroyRtmp(RTMP **pprtmp)
{
	RTMP *rtmp = *pprtmp;

	if (rtmp != NULL) {
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		*pprtmp = NULL;
	}

	return 0;
}

/************************************************************************************************/

CFetchRtmpStream::CFetchRtmpStream()
{
}

CFetchRtmpStream::~CFetchRtmpStream()
{
}

void CFetchRtmpStream::SetParams(void *context, PACKETCALLBACK pcbk, string url)
{
	m_context = context;
	m_pcbk = pcbk;
	m_url = url;

	return;
}

void CFetchRtmpStream::Loop()
{
	int nRead;
	bool bLiveStream = true;
	int bufsize = 1024*1024*10;
	char *buf = (char*)malloc(bufsize);
	long countbufsize = 0;

	memset(buf, 0, bufsize);

	CreateRtmp(m_url, &m_rtmp);

	while((nRead = RTMP_Read(m_rtmp, buf, bufsize)) > 0) {
		if(m_pcbk != NULL) {
			m_pcbk(m_context, buf, nRead);
		}

		countbufsize += nRead;
		RTMP_LogPrintf("Receive: %5d Byte, Total: %5.2f kB\n", nRead, countbufsize*1.0/1024);
	}

	DestroyRtmp(&m_rtmp);

	if(buf) {
		free(buf);
		buf = NULL;
	}

	return;
}

} // end namespace


