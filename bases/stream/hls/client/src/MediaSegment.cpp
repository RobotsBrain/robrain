#include <stdio.h>

#include "Common.h"
#include "MediaSegment.h"



namespace Hls {


MediaSegment::MediaSegment()
: m_uri("")
, m_discontinuity(false)
, m_duration(0) 
{
}

MediaSegment::~MediaSegment()
{
}

int MediaSegment::parseMediaSegemnt(const std::string &line) 
{ 
    if(startWith(line, EXTINF)) {
        // parse Segment Duration
        int colonPos = line.find(":");
        if(colonPos >= 0) {
            parseDouble(line.c_str() + colonPos + 1, &m_duration);
        }      
    }

    if(startWith(line, EXT_X_DISCONTINUITY)) {
        m_discontinuity = true;
    }

    if(!startWith(line, "#")) {
        m_uri = line;
    }

    printf("\n<-------mediasegement------->\n");
    printf("duration : %f\n", m_duration);
    printf("uri : %s \n", m_uri.c_str());
    printf("<--------------------------->\n\n");

    return 0;
}

std::string MediaSegment::getUri() 
{
    return m_uri;
}

int MediaSegment::getDuration() 
{
    return m_duration;
}

} // end namespace


