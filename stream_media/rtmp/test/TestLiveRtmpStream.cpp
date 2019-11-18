#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <string>

#include "rbav/RbavHeader.h"
#include "flv/Flv.h"
#include "flv/IFlvMux.h"
#include "LiveRtmpStream.h"

using namespace std;
using namespace Flv;
using namespace Rtmp;



class CTestFlvMux : public IFlvDataCallback
{
public:
	CTestFlvMux();
	~CTestFlvMux();
	
	virtual void FlvPacket(const void *context, const u_char *data, const int size, const int pretagsize);
};

CTestFlvMux::CTestFlvMux()
{
}

CTestFlvMux::~CTestFlvMux()
{
}

void CTestFlvMux::FlvPacket(const void *context, const u_char *data, const int size, const int pretagsize)
{
	CLiveRtmpStream *pstream = (CLiveRtmpStream *)context;

	if(pstream != NULL) {
		pstream->SendStream((char *)data, size);
	}

	return;
}

bool ReadFlvTagHeader(FILE *fp, TagHeader &tag)
{
	u_char buf[11] = {0};

	// jump over previousTagSize
	if(fseek(fp, 4, SEEK_CUR) < 0) {
		printf("seek fail!\n");
		return false;
	}

	size_t size = fread(buf, 1, 11, fp);
	if(size != 11) {
		printf("can not read request size data!\n");
		return false;
	}

	memset(&tag, 0, sizeof(TagHeader));

	tag.nType = buf[0];
	tag.nDataSize = (buf[1] << 16) | (buf[2] << 8) | buf[3];
	tag.nTimeStamp = (buf[4] << 16) | (buf[5] << 8) | buf[6];
	tag.nTSEx = buf[7];
	tag.nStreamID = (buf[8] << 16) | (buf[9] << 8) | buf[10];

	return true;
}

void LiveFlvFile(FILE *fp, char *pFileBuf, CLiveRtmpStream *stream)
{
	TagHeader tag;

	fseek(fp, 9, SEEK_SET); //jump over FLV Header

	while(1) {
		if(feof(fp) != 0) {
			printf("goto file head!\n");
			fseek(fp, 9, SEEK_SET);
		}

		if(ReadFlvTagHeader(fp, tag) == false) {
			fseek(fp, 9, SEEK_SET);
			continue;
		}

		if (tag.nType != 0x08 && tag.nType != 0x09) {
			//jump over non audio/video frame，
			fseek(fp, tag.nDataSize, SEEK_CUR);
			continue;
		}

		int len = 11 + tag.nDataSize;

		fseek(fp, -11, SEEK_CUR); //jump back

		memset(pFileBuf, 0, len);

		if(fread(pFileBuf, 1, len, fp) != len) {
			printf("read data error\n");
			break;
		}

		stream->SendStream(pFileBuf, len);

		usleep(25 * 1000);
	}

	return;
}

uint64_t CurrentUtcTimeMs()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return ((uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void LiveRawFile(FILE *fp, char *pFileBuf, CLiveRtmpStream *stream)
{
	int len = 0;
	RbAvHeader avHeader;
	CTestFlvMux testmux;
	IFlvMux *mux = IFlvMux::Create();
	uint64_t start_av_time = 0;
	uint64_t start = CurrentUtcTimeMs();

	mux->SetCallback(&testmux, stream);
	mux->Start(true, true);

	while (1) {
		if(feof(fp) != 0) {
			printf("goto file head!\n");
			fseek(fp, 0, SEEK_SET);
		}

		if((len = fread((char *)&avHeader, 1, sizeof(RbAvHeader), fp)) != sizeof(RbAvHeader)) {
			printf("get header info, return zero\n");
			break;
		}

		if((len = fread(pFileBuf, 1, avHeader.size, fp)) != avHeader.size) {
			printf("get data, return zero\n");
			break;
		}

		if(avHeader.type == AV_AUDIO) {
			mux->ConvertAAC((u_char *)pFileBuf, avHeader.size, avHeader.timestamp);
		} else if(avHeader.type == AV_VIDEO) {
			mux->ConvertH264((u_char *)pFileBuf, avHeader.size, avHeader.timestamp);
		}

		if(start_av_time == 0) {
			start_av_time = avHeader.timestamp;
		}

		uint64_t cur = CurrentUtcTimeMs();
		uint64_t interval = cur - start;
		uint64_t av_interval = avHeader.timestamp - start_av_time;

		if(av_interval > interval) {
			printf("sleep %ld ms\n", av_interval - interval);
			usleep((av_interval - interval) * 1000);
		} else {
			usleep(10 * 1000);
		}
	}

	mux->Stop();
	mux->Destroy();

	return;
}

void StreamSend(int mode, string filename)
{
	FILE *fp = NULL;
	CLiveRtmpStream stream;

	stream.SetParams("rtmp://localhost:1935/live/livestream");
	stream.Start();

	fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		printf("Open File Error.\n");
		return;
	}

	char *pFileBuf = (char *)malloc(1024 * 1024);
	if(pFileBuf == NULL) {
		return;
	}

	if(mode == 1) {
		LiveRawFile(fp, pFileBuf, &stream);
	} else {
		LiveFlvFile(fp, pFileBuf, &stream);
	}

	if(pFileBuf != NULL) {
		free(pFileBuf);
		pFileBuf = NULL;
	}

	stream.Stop();

	if (fp) {
		fclose(fp);
	}

	return;
}

int main(int argc, char **argv) 
{
	int res = 0;
	int mode = 0;
	string infile;

	while((res = getopt(argc, argv, "?m:f:h")) != -1) {
        switch(res) {
        case 'm':
        	mode = atoi(optarg);
        	break;

        case 'f':
			infile = optarg;
			break;

		case 'h':
			break;

        default:
			break;
        }
    }

    StreamSend(mode, infile);

	return 0;
}


