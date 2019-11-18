#ifndef __FLVDUMPFILE_H__
#define __FLVDUMPFILE_H__

#include <string>

#include "Flv.h"



void PrintInfo(CFlv *pFlvParser);

int DumpH264(CFlv *pFlvParser, const std::string &path);
int DumpAAC(CFlv *pFlvParser, const std::string &path);
int DumpFlv(CFlv *pFlvParser, const std::string &path);

#endif
