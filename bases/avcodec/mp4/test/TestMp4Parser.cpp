#include <stdio.h>
#include <unistd.h>

#include "Mp4Parser.h"



class CDemo : public CMp4ParserCallBack 
{
public:
	CDemo();
	virtual ~CDemo();

	bool StartParse();
	bool StopParse();

public:
	virtual void OnVideoCallBack(void *thiz, const u_char *pPacketPtr, const uint16_t wPackLen, const bool bKeyFrame);
	virtual void OnAudioCallBack(void *thiz,  const u_char *pHeader, const uint16_t wHeaderSize, 
								const u_char *pPacketPtr, const uint16_t wPackLen);

private:
	CMp4Parser *pFile;
	FILE *pOutVideoFile;
	FILE *pOutAudioFile;
};

CDemo::CDemo()
{
	pFile = new CMp4Parser(this, "test.mp4");

	pOutVideoFile = fopen("out.h264", "wb");
	pOutAudioFile = fopen("out.aac", "wb");
}

CDemo::~CDemo()
{
	if (pFile) {
		delete pFile;
		pFile = NULL;
	}
}

bool CDemo::StartParse()
{
	return pFile->Start();
}

bool CDemo::StopParse()
{
	return pFile->Stop();
}

void CDemo::OnVideoCallBack(void *thiz, const u_char *pPacketPtr, const uint16_t wPackLen, const bool bKeyFrame)
{
	fwrite(pPacketPtr, 1, wPackLen, pOutVideoFile);
}

void CDemo::OnAudioCallBack(void *thiz,  const u_char *pHeader, const uint16_t wHeaderSize, 
							const u_char *pPacketPtr, const uint16_t wPackLen)
{
	fwrite(pHeader, 1, wHeaderSize, pOutAudioFile);
	fwrite(pPacketPtr, 1, wPackLen, pOutAudioFile);
}

int main(int argc, char **argv)
{
	CDemo *pDemo = new CDemo();
	if (NULL == pDemo) {
		return -1;
	}

	pDemo->StartParse();

	pDemo->StopParse();

	delete pDemo;
	pDemo = NULL;

	return 0;
}


