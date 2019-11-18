#ifndef __FLVPARSER_H__
#define __FLVPARSER_H__

#include <stdint.h>
#include <string>
#include <vector>

#include "Flv.h"
#include "Tag.h"



class CFlvParser : public CFlv
{
public:
	CFlvParser();
	virtual ~CFlvParser();

	int Parse(u_char *pBuf, int nBufSize, int &nUsedLen);

private:
	CTag *CreateTag(u_char *pBuf, int nLeftLen);

private:
	bool m_bFlvHeader;
};

#endif
