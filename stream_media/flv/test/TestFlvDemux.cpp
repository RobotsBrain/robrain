#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

#include "rbav/RbavHeader.h"
#include "flv/IFlvDemux.h"

using namespace std;



static const unsigned int nH264StartCode = 0x01000000;

static void WriteU64(uint64_t &x, int length, int value) 
{
	uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
	x = (x << length) | ((uint64_t) value & mask);
}

class TestFlvDemux : public Flv::IAVDataCallback
{
public:
	TestFlvDemux(int mode, string infile);
	~TestFlvDemux();
	
	virtual void SpsPps(const void *context, const char *sps, const int sps_size, const char *pps, const int pps_size);
	virtual void H264Nalu(const void *context, const char *nalu, const int size, const uint32_t timestamp);

	virtual void AudioSpecific(const void *context, const int profile, const int sampleRateIndex, const int channel);
	virtual void AacPacket(const void *context, const char *aac, const int size, const uint32_t timestamp);

	void Parser();

private:
	fstream m_fin;
	fstream m_h264;
	fstream m_aac;
	fstream m_self_raw;

	int m_mode;

	int m_profile;
	int m_sampleRateIndex;
	int m_channel;
};


TestFlvDemux::TestFlvDemux(int mode, string infile)
: m_mode(mode)
{
	m_fin.open(infile.c_str(), ios_base::in | ios_base::binary);

	if(m_mode) {
		m_h264.open("test.h264", ios_base::out | ios_base::binary);
		m_aac.open("test.aac", ios_base::out | ios_base::binary);
	} else {
		m_self_raw.open("test.raw", ios_base::out | ios_base::binary);
	}
}

TestFlvDemux::~TestFlvDemux()
{
	if(m_mode) {
		m_h264.close();
		m_aac.close();
	} else {
		m_self_raw.close();
	}

	m_fin.close();
}

void TestFlvDemux::SpsPps(const void *context, const char *sps, const int sps_size, 
							const char *pps, const int pps_size)
{
	if(m_mode) {
		m_h264.write((const char*)&nH264StartCode, 4);
		m_h264.write(sps, sps_size);
		m_h264.write((const char*)&nH264StartCode, 4);
		m_h264.write(pps, pps_size);
	} else {
		RbAvHeader avHeader;

		avHeader.type = AV_VIDEO;
		avHeader.timestamp = 0;
		avHeader.size = 4 + sps_size;

		m_self_raw.write((const char *)&avHeader, sizeof(RbAvHeader));
		m_self_raw.write((const char *)&nH264StartCode, 4);
		m_self_raw.write(sps, sps_size);

		avHeader.type = AV_VIDEO;
		avHeader.timestamp = 0;
		avHeader.size = 4 + pps_size;

		m_self_raw.write((const char *)&avHeader, sizeof(RbAvHeader));
		m_self_raw.write((const char *)&nH264StartCode, 4);
		m_self_raw.write(pps, pps_size);
	}

	return;
}

void TestFlvDemux::H264Nalu(const void *context, const char *nalu, const int size, const uint32_t timestamp)
{
	if(m_mode) {
		m_h264.write((const char*)&nH264StartCode, 4);
		m_h264.write(nalu, size);
	} else {
		RbAvHeader avHeader;

		avHeader.type = AV_VIDEO;
		avHeader.timestamp = timestamp;
		avHeader.size = 4 + size;

		m_self_raw.write((const char *)&avHeader, sizeof(RbAvHeader));
		m_self_raw.write((const char*)&nH264StartCode, 4);
		m_self_raw.write(nalu, size);
	}

	// printf("			video timestamp: %d\n", timestamp);

	return;
}

void TestFlvDemux::AudioSpecific(const void *context, const int profile, 
									const int sampleRateIndex, const int channel)
{
	m_profile = profile;
	m_sampleRateIndex = sampleRateIndex;
	m_channel = channel;

	printf("profile: %d, sampleRateIndex: %d, channel: %d\n", m_profile, m_sampleRateIndex, m_channel);

	return;
}

void TestFlvDemux::AacPacket(const void *context, const char *aac, const int size, const uint32_t timestamp)
{
	uint64_t bits = 0;

	WriteU64(bits, 12, 0xFFF);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 2, 0);
	WriteU64(bits, 1, 1);
	WriteU64(bits, 2, m_profile);
	WriteU64(bits, 4, m_sampleRateIndex);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 3, m_channel);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 13, 7 + size);
	WriteU64(bits, 11, 0x7FF);
	WriteU64(bits, 2, 0);

	u_char p64[8];

	p64[0] = (u_char)(bits >> 56);
	p64[1] = (u_char)(bits >> 48);
	p64[2] = (u_char)(bits >> 40);
	p64[3] = (u_char)(bits >> 32);
	p64[4] = (u_char)(bits >> 24);
	p64[5] = (u_char)(bits >> 16);
	p64[6] = (u_char)(bits >> 8);
	p64[7] = (u_char)(bits);

	if(m_mode) {
		m_aac.write((const char *)p64 + 1, 7);
		m_aac.write(aac, size);
	} else {
		RbAvHeader avHeader;

		avHeader.type = AV_AUDIO;
		avHeader.timestamp = timestamp;
		avHeader.size = 7 + size;

		m_self_raw.write((const char *)&avHeader, sizeof(RbAvHeader));
		m_self_raw.write((const char *)p64 + 1, 7);
		m_self_raw.write(aac, size);
	}

	// printf("audio timestamp: %d\n", timestamp);

	return;
}

void TestFlvDemux::Parser()
{
	int nBufSize = 2000 * 1024;
	int nFlvPos = 0;
	unsigned char *pBuf, *pBak;
	pBuf = new unsigned char[nBufSize];
	pBak = new unsigned char[nBufSize];
	Flv::IFlvDemux *parser = Flv::IFlvDemux::Create();

	parser->SetCallback(this, this);

	while (1) {
		int nReadNum = 0;
		int nUsedLen = 0;
		m_fin.read((char *)pBuf + nFlvPos, nBufSize - nFlvPos);
		nReadNum = m_fin.gcount();
		if (nReadNum == 0) {
			break;
		}

		nFlvPos += nReadNum;

		parser->Parser(pBuf, nFlvPos, nUsedLen);
		if (nFlvPos != nUsedLen) {
			memcpy(pBak, pBuf + nUsedLen, nFlvPos - nUsedLen);
			memcpy(pBuf, pBak, nFlvPos - nUsedLen);
		}

		nFlvPos -= nUsedLen;
	}

	delete[] pBak;
	delete[] pBuf;

	parser->Destroy();

	return;
}

int main(int argc, char *argv[])
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

	TestFlvDemux parser(mode, infile.c_str());

	parser.Parser();

	return 0;
}


