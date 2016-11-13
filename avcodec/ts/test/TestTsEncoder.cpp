#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <fstream>

#include "base/Log.h"

#include "Mux.h"

using namespace std;

int GetOneNalu(u_char * pBufIn, int nInSize, u_char * pNalu, int &nNaluSize)
{
	u_char *p = pBufIn;
	int nStartPos = 0, nEndPos = 0;

	while (1) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
			nStartPos = p - pBufIn;
			break;
		}

		p++;

		if (p - pBufIn >= nInSize - 4) {
			return 0;
		}
	}

	p++;

	while (1) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
			nEndPos = p - pBufIn;
			break;
		}

		p++;

		if (p - pBufIn >= nInSize - 4) {
			nEndPos = nInSize;
			break;
		}
	}

	nNaluSize = nEndPos - nStartPos;
	memcpy(pNalu, pBufIn + nStartPos, nNaluSize);

	return 1;
}

int GetOneAACFrame(u_char * pBufIn, int nInSize, u_char * pAACFrame,
				   int &nAACFrameSize)
{
	unsigned char *p = pBufIn;

	if (nInSize <= 7)
		return 0;

	int nFrameSize = ((p[3] & 0x3) << 11) + (p[4] << 3) + (p[5] >> 5);
	if (nInSize < nFrameSize)
		return 0;

	nAACFrameSize = nFrameSize;
	memcpy(pAACFrame, pBufIn, nFrameSize);

	return 1;
}

class CVideo {
  public:
	CVideo(string filename);
	~CVideo();

	int Write();

  private:
	int m_TimeStamp;
	fstream m_File;
	int m_nFileSize;
	u_char *m_pBufferIn;
	u_char *m_pBufferOut;
	int m_nOffset;
	int m_count;
};

CVideo::CVideo(string filename)
: m_TimeStamp(0)
, m_File(NULL)
, m_nFileSize(0)
, m_pBufferIn(NULL)
, m_pBufferOut(NULL)
, m_nOffset(0)
, m_count(0)
{
	m_File.open(filename.c_str(), ios::binary | ios::in);

	m_File.seekg(0, ios::end);
	m_nFileSize = m_File.tellg();

	m_File.seekg(0, ios_base::beg);

	m_pBufferIn = new u_char[m_nFileSize];
	m_pBufferOut = new u_char[m_nFileSize];

	m_File.read((char *)m_pBufferIn, m_nFileSize);
}

CVideo::~CVideo()
{
	delete m_pBufferIn;
	delete m_pBufferOut;

	m_File.close();
}

int CVideo::Write()
{
	int nNaluSize = 0;

	if (GetOneNalu(m_pBufferIn + m_nOffset, m_nFileSize - m_nOffset, m_pBufferOut,
		 nNaluSize) == 0) {
		DEBUG("\n");
		return -1;
	}

	WriteBuf2TsFile(25, 1, m_pBufferOut, nNaluSize, 0);

	if (m_pBufferOut[4] != 0x67 && m_pBufferOut[4] != 0x68) {
		m_TimeStamp += 33;
	}

	m_nOffset += nNaluSize;

	if (m_nOffset >= m_nFileSize - 4) {
		DEBUG("\n");
		return -1;
	}

	m_count++;

	return m_TimeStamp;
}

class CAudio {
  public:
	CAudio(string filename);
	~CAudio();

	int Write();

  private:
	int m_TimeStamp;
	fstream m_File;
	int m_nFileSize;
	u_char *m_pBufferIn;
	u_char *m_pBufferOut;
	int m_nOffset;
	int m_count;
};

CAudio::CAudio(string filename)
: m_TimeStamp(0)
, m_File(NULL)
, m_nFileSize(0)
, m_pBufferIn(NULL)
, m_pBufferOut(NULL)
, m_nOffset(0)
, m_count(0)
{
	m_File.open(filename.c_str(), ios::binary | ios::in);

	m_File.seekg(0, ios::end);
	m_nFileSize = m_File.tellg();

	m_File.seekg(0, ios_base::beg);

	m_pBufferIn = new u_char[m_nFileSize];
	m_pBufferOut = new u_char[m_nFileSize];

	m_File.read((char *)m_pBufferIn, m_nFileSize);
}

CAudio::~CAudio()
{
	delete m_pBufferIn;
	delete m_pBufferOut;

	m_File.close();
}

int CAudio::Write()
{
	int nAACFrameSize = 0;

	if (GetOneAACFrame
		(m_pBufferIn + m_nOffset, m_nFileSize - m_nOffset, m_pBufferOut,
		 nAACFrameSize) == 0) {
		DEBUG("\n");
		return -1;
	}

	printf("nAACFrameSize = %d\n", nAACFrameSize);
	
	WriteBuf2TsFile(8, 0, m_pBufferOut, nAACFrameSize, 0);

	m_TimeStamp += double (1024 * 1000) / double (44100);
	m_nOffset += nAACFrameSize;

	if (m_nOffset >= m_nFileSize - 4) {
		DEBUG("\n");
		return -1;
	}

	m_count++;

	return m_TimeStamp;
}

int main(int argc, char *argv[])
{
	int ch;
	string videofile;
	string audiofile;
	bool bAudio = false, bVideo = false;

	while ((ch = getopt(argc, argv, "a:v:")) != -1) {

		switch (ch) {
		case 'a':
			audiofile = optarg;
			bAudio = true;
			break;

		case 'v':
			videofile = optarg;
			bVideo = true;
			break;

		default:
			break;
		}
	}

	CAudio *pAudio = NULL;
	CVideo *pVideo = NULL;
	uint32_t atime = 0, vtime = 0;
	bool bastop = false, bvstop = false;

	if (bAudio) {
		pAudio = new CAudio(audiofile);
	}

	if (bVideo) {
		pVideo = new CVideo(videofile);
	}

	int timestamp = 0;

	pVideo_Audio_Ts_File = OpenFile("h264_aac.ts", "wb");

	while (1) {
		if (bAudio && bVideo) {
			if (atime <= vtime && !bastop) {
				if ((timestamp = pAudio->Write()) < 0) {
					bastop = true;
				} else {
					atime = timestamp;
				}
			} else if (!bvstop) {
				if ((timestamp = pVideo->Write()) < 0) {
					bvstop = true;
				} else {
					vtime = timestamp;
				}
			}

			if (bastop && bvstop) {
				break;
			}
		} else if (bAudio) {
			if ((timestamp = pAudio->Write()) < 0) {
				break;
			}

			atime = timestamp;
		} else if (bVideo) {
			if ((timestamp = pVideo->Write()) < 0) {
				break;
			}

			vtime = timestamp;
		}
	}

	if (pVideo_Audio_Ts_File)
  	{
	  	CloseFile(pVideo_Audio_Ts_File);
	  	pVideo_Audio_Ts_File = NULL;
  	}

	if (pAudio != NULL)
		delete pAudio;

	if (pVideo != NULL)
		delete pVideo;

	return 0;
}


