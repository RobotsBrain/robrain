#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

#include "rbav/RbavHeader.h"
#include "flv/IFlvMux.h"

using namespace std;



class CTestFlvMux : public Flv::IFlvDataCallback
{
public:
	CTestFlvMux();
	~CTestFlvMux();
	
	virtual void FlvPacket(const void *context, const u_char *data, const int size, const int pretagsize);

	void MuxData();

private:
	fstream m_flv;
	fstream m_self_raw;
};

CTestFlvMux::CTestFlvMux()
{
	m_flv.open("mux.flv", ios_base::out | ios_base::binary);
	m_self_raw.open("test.raw", ios_base::in | ios_base::binary);
}

CTestFlvMux::~CTestFlvMux()
{
	m_flv.close();
	m_self_raw.close();
}

void CTestFlvMux::FlvPacket(const void *context, const u_char *data, const int size, const int pretagsize)
{
	m_flv.write((char *)data, size);
	m_flv.write((char *)&pretagsize, sizeof(int));
}

void CTestFlvMux::MuxData()
{
	RbAvHeader avHeader;
	char buf[1024 * 1024] = {0};
	Flv::IFlvMux *mux = Flv::IFlvMux::Create();

	mux->SetCallback(this, this);
	mux->Start(true, true);

	while (1) {
		if(m_self_raw.read((char *)&avHeader, sizeof(RbAvHeader)) == 0) {
			printf("get header info, return zero\n");
			break;
		}

		if(m_self_raw.read(buf, avHeader.size) == 0) {
			printf("get data, return zero\n");
			break;
		}

		if(avHeader.type == AV_AUDIO) {
			mux->ConvertAAC((u_char *)buf, avHeader.size, avHeader.timestamp);
		} else if(avHeader.type == AV_VIDEO) {
			mux->ConvertH264((u_char *)buf, avHeader.size, avHeader.timestamp);
		}
	}

	mux->Stop();
	mux->Destroy();

	return;
}

int main(int argc, char **argv)
{
	CTestFlvMux mux;

	mux.MuxData();

	return 0;
}