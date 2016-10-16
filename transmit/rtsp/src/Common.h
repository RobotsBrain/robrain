#ifndef __COMMON_H__
#define __COMMON_H__



namespace Rtsp {

unsigned int Random32(int type);

unsigned long GetRanddomSeq();

void AddTimestamp(char *b, int crlf);

} // end namespace


#endif