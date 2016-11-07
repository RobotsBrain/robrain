#ifndef __FLVPARSER_H__
#define __FLVPARSER_H__

#include <stdint.h>
#include <string>
#include <vector>

#include "Flv.h"
#include "Tag.h"



class CFlvParser : public CFlv {
public:
	CFlvParser();
	virtual ~CFlvParser();

	int Parse(u_char *pBuf, int nBufSize, int &nUsedLen);

	void GetTags(std::vector <CTag *> &vpTag);
	void GetFlvHeader(FlvHeader &flvHeader);

	void PrintFlvHeader();

private:
	FlvHeader *CreateFlvHeader(u_char *pBuf);
	void DestroyFlvHeader();

	CTag *CreateTag(u_char *pBuf, int nLeftLen);
	void DestroyTag();

private:
	FlvHeader 				*m_pFlvHeader;
	std::vector <CTag *>	m_vpTag;
};

#endif // FLVPARSER_H
