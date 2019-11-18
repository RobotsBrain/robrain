#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "FetchRtmpStream.h"

using namespace Rtmp;

#define RTMP_URL "rtmp://live.hkstv.hk.lxdns.com/live/hks"



void DataCallback(void *context, char *data, int size)
{
	FILE *fp = (FILE *)context;

	if(fp != NULL) {
		fwrite(data, 1, size, fp);
	}

	return;
}

int main(int argc, char* argv[])
{
	CFetchRtmpStream fetch;
	FILE *fp = NULL;

	fp = fopen("receive.flv", "wb");
	if (!fp) {
		RTMP_LogPrintf("Open File Error.\n");
		return -1;
	}

	fetch.SetParams((void *)fp, DataCallback, RTMP_URL);

	fetch.Loop();
	
	if(fp) {
		fclose(fp);
	}

	return 0;
}


