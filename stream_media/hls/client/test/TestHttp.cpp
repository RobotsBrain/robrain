#include "Http.h"



int main(int argc, char **argv)
{
	Hls::CHttp *http = new Hls::CHttp();

	// test local http server
	// http->Request("http://127.0.0.1:8080/test.m3u8");
	http->Request("www.baidu.com");
	http->Request("http://live.hkstv.hk.lxdns.com/live/hks/playlist.m3u8");
	http->Request("http://dlhls.cdn.zhanqi.tv/zqlive/34338_PVMT5.m3u8");
	
	delete http;

	return 0;
}