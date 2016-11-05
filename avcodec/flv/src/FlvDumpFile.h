#ifndef __FLVDUMPFILE_H__
#define __FLVDUMPFILE_H__

#include <string>

#include "FlvParser.h"



void PrintInfo(CFlvParser *pFlvParser);

int DumpH264(CFlvParser *pFlvParser, const std::string &path);
int DumpAAC(CFlvParser *pFlvParser, const std::string &path);
int DumpFlv(CFlvParser *pFlvParser, const std::string &path);

#endif
