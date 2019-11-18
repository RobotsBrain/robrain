#ifndef __M3U8PARSER_H__
#define __M3U8PARSER_H__

#include <vector>

#include "MediaSegment.h"
#include "M3u8Media.h"


namespace Hls {

class M3U8Parser 
{
public:
    M3U8Parser();
    ~M3U8Parser();

    M3UMedia* getM3uMedia();

    int parser(const char* data, int size);

private:
    int playListType;
    bool m_isM3U8File;
    int m3u8Version;
    M3UMedia *m3uMedia;
    std::vector<MediaSegment*> mMediaSegmentVector;
};

} // end namespace

#endif