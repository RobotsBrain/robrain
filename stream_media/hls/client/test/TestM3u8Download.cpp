#include "M3u8Download.h"



int main(int argc, char **argv)
{
	Hls::CM3u8Download *dl = new Hls::CM3u8Download();

	dl->FetchM3u8File("http://dlhls.cdn.zhanqi.tv/zqlive/34338_PVMT5.m3u8");

	delete dl;

	return 0;
}