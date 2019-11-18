#ifndef __M3U8DOWNLOAD_H__
#define __M3U8DOWNLOAD_H__

#include <string>

using namespace std;



namespace Hls {

class CM3u8Download
{
public:
	CM3u8Download();
	~CM3u8Download();
	
	void FetchM3u8File(std::string url);
};

}

#endif