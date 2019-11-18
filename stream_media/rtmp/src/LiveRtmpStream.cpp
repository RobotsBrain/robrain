#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <string>

#include "LiveRtmpStream.h"

using namespace std;


namespace Rtmp {

bool CreateRtmp(string url, RTMP **pprtmp)
{
	RTMP *rtmp = NULL;

	rtmp = RTMP_Alloc();
	RTMP_Init(rtmp);

	rtmp->Link.timeout = 5;

	if(!RTMP_SetupURL(rtmp, (char *)url.c_str())) {
		RTMP_Free(rtmp);
		return false;
	}

	RTMP_EnableWrite(rtmp);

	if(!RTMP_Connect(rtmp, NULL)) {
		RTMP_Free(rtmp);
		return false;
	}

	if(!RTMP_ConnectStream(rtmp, 0)) {
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		return false;
	}
		
	RTMP_SetBufferMS(rtmp, 3600 * 1000); // 1hour
	
	*pprtmp = rtmp;

	return true;
}

bool DestroyRtmp(RTMP **pprtmp)
{
	RTMP *rtmp = *pprtmp;

	if (rtmp != NULL) {
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		*pprtmp = NULL;
	}

	return true;
}

/***********************************************************************************/

static void *ThreadProc(void *arg);

CLiveRtmpStream::CLiveRtmpStream()
: m_url("")
{
}

CLiveRtmpStream::~CLiveRtmpStream()
{
}

void CLiveRtmpStream::SetParams(string url)
{
	m_url = url;

	printf("url(%s)\n", url.c_str());

	return;
}

bool CLiveRtmpStream::Start()
{
	printf("\n");

    CreateRtmp(m_url, &m_rtmp);

	return true;
}

bool CLiveRtmpStream::Stop()
{
	printf("\n");

	DestroyRtmp(&m_rtmp);

	return true;
}

int CLiveRtmpStream::SendStream(char *buf, uint32_t size)
{
	do {
		if (!RTMP_IsConnected(m_rtmp)) {
			break;
		}

		if (!RTMP_Write(m_rtmp, buf, size)) {
			break;
		}
	} while(0);

	return 0;
}

} // end namespace