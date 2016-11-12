#include <stdio.h>
#include <unistd.h>

#include "Mp4Parser.h"



class CDemo : public CMp4FileCallBack 
{
public:
	CDemo();
	virtual ~CDemo();

	uint16_t StartParse();
	uint16_t StopParse();

public:
	virtual void OnVideoCallBack(void *thiz, const u_char *pPacketPtr, const uint16_t wPackLen, const bool bKeyFrame);
	virtual void OnAudioCallBack(void *thiz, const u_char *pPacketPtr, const uint16_t wPackLen);

private:
	CMp4File *pFile;
	FILE *pOutVideoFile;
	FILE *pOutAudioFile;
};

CDemo::CDemo()
{
	pFile = new CMp4File(this, "test.mp4");

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

uint16_t CDemo::StartParse()
{
	if (NULL == pFile) {
		return 1;
	}

	return pFile->StartParse();
}

uint16_t CDemo::StopParse()
{
	if (NULL == pFile) {
		return 1;
	}

	return pFile->StopParse();
}

void CDemo::OnVideoCallBack(void *thiz, const u_char *pPacketPtr, const uint16_t wPackLen, const bool bKeyFrame)
{
	if (pOutVideoFile && pPacketPtr) {
		fwrite(pPacketPtr, 1, wPackLen, pOutVideoFile);
	}
}

void CDemo::OnAudioCallBack(void *thiz, const u_char *pPacketPtr, const uint16_t wPackLen)
{
	if (pOutAudioFile && pPacketPtr) {
		fwrite(pPacketPtr, 1, wPackLen, pOutAudioFile);
	}
}

int main(int argc, char **argv)
{
	uint16_t wRet = MP4PARSER_NO_ERROR;

	CDemo *pDemo = new CDemo();
	if (NULL == pDemo) {
		return -1;
	}

	wRet = pDemo->StartParse();
	if (MP4PARSER_NO_ERROR != wRet) {
		return -1;
	}

	pDemo->StopParse();
	delete pDemo;
	pDemo = NULL;

	return 0;
}


