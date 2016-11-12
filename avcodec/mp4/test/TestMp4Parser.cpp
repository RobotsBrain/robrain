#include <stdio.h>
#include <unistd.h>

#include "mp4parser.h"



class CDemo : public CMp4FileCallBack 
{
public:
	CDemo();
	virtual ~CDemo();

	unsigned short SetFile(const char *pInFile);

	unsigned short StartParse();
	unsigned short StopParse();

public:
	virtual void OnVideoCallBack(const CMp4File * pcParser,
								 const unsigned char *pPacketPtr,
								 const unsigned short wPackLen,
								 const bool bKeyFrame);

	virtual void OnAudioCallBack(const CMp4File * pcParser,
								 const unsigned char *pPacketPtr,
								 const unsigned short wPackLen);

private:
	CMp4File * pFile;
	FILE *pOutVideoFile;
	FILE *pOutAudioFile;
};

CDemo::CDemo()
{
	pFile = new CMp4File(this);

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

unsigned short CDemo::SetFile(const char *pInFile)
{
	if (NULL == pFile) {
		return 1;
	}

	return pFile->SetFile(pInFile);
}

unsigned short CDemo::StartParse()
{
	if (NULL == pFile) {
		return 1;
	}

	return pFile->StartParse();
}

unsigned short CDemo::StopParse()
{
	if (NULL == pFile) {
		return 1;
	}

	return pFile->StopParse();
}

void CDemo::OnVideoCallBack(const CMp4File * pcParser,
							const unsigned char *pPacketPtr,
							const unsigned short wPackLen, const bool bKeyFrame)
{
	if (pOutVideoFile && pPacketPtr) {
		fwrite(pPacketPtr, 1, wPackLen, pOutVideoFile);
	}
}

void CDemo::OnAudioCallBack(const CMp4File * pcParser,
							const unsigned char *pPacketPtr,
							const unsigned short wPackLen)
{
	if (pOutAudioFile && pPacketPtr) {
		fwrite(pPacketPtr, 1, wPackLen, pOutAudioFile);
	}
}

CDemo *g_pDemo_1 = NULL;

void quit()
{
	if (g_pDemo_1) {
		g_pDemo_1->StopParse();
		delete g_pDemo_1;
		g_pDemo_1 = NULL;
	}
}

int main(int argc, char **argv)
{
	unsigned short wRet = MP4PARSER_NO_ERROR;

	g_pDemo_1 = new CDemo();

	if (NULL == g_pDemo_1) {
		quit();
		return -1;
	}

	wRet = g_pDemo_1->SetFile("test.mp4");
	if (MP4PARSER_NO_ERROR != wRet) {
		quit();
		return -1;
	}

	wRet = g_pDemo_1->StartParse();
	if (MP4PARSER_NO_ERROR != wRet) {
		quit();
		return -1;
	}

	while(1) {
		sleep(5);
	}

	return 0;
}


