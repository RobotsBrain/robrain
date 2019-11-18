#include <iostream>
#include <map>
#include <list>
#include <vector>

#include "MediaSegment.h"
#include "Common.h"
#include "M3u8Media.h"
#include "M3u8Parser.h"

enum {
    M3U8_TYPE_UNKNOWN = 0,
    M3U8_TYPE_MASTER = 1,
    M3U8_TYPE_MEDIA = 2
};

enum {
    TAG_TYPE_EXTM3U = 1,
    TAG_TYPE_X_VERSION = 2,
};

namespace Hls {

bool isMediaSegmentTag(std::string &line)
{
    if(startWith(line, EXTINF) || startWith(line, EXT_X_BYTERANGE) 
        || startWith(line, EXT_X_DISCONTINUITY) || startWith(line, EXT_X_KEY) 
        || startWith(line, EXT_X_MAP) || startWith(line, EXT_X_PROGRAM_DATE_TIME) 
        || startWith(line, EXT_X_BYTERANGE)) {

        return true;
    }
    
    return false;
}

bool parseM3UVersion(const std::string &line, int *x) 
{
    int colonPos = line.find(":");

    if(colonPos < 0) {
        return false; 
    }

    bool ret = parseInt32(line.c_str() + colonPos + 1, x);

    return ret;
}

M3U8Parser::M3U8Parser()
: playListType(M3U8_TYPE_UNKNOWN)
, m3u8Version(-1)
, m3uMedia(NULL)
, m_isM3U8File(false)
{
}

M3U8Parser::~M3U8Parser()
{
}

M3UMedia* M3U8Parser::getM3uMedia() 
{
    return m3uMedia;
}

int M3U8Parser::parser(const char *data, int size) 
{
    int offset = 0;
    int lineNo = -1;
    MediaSegment *curMediaSegment = NULL;

    std::cout<<size<<std::endl;
    std::cout<<data<<std::endl;

    while(offset < size) {
        std::string line = getLine(data, &offset, size);
        lineNo++;

        //the first line of m3u file must be "#EXTM3U"
        if(lineNo == 0) {
            if(line == EXTM3U) {
                m_isM3U8File = true;
                std::cout<<"this is a m3u8 file\n";
                continue;
            } else {
                return ERROR_NOT_M3U8_FILE;
            }
        } 

        // lineNo >= 1
        if(startWith(line, EXT_X_VERSION)) {
            int v = -1;

            if(parseM3UVersion(line, &v)) {
                m3u8Version = v;
                std::cout<<"m3u8 version is "<<m3u8Version<<std::endl;
            }

            continue;
        }

        //parse master m3u8
        if(startWith(line, EXT_X_STREAM_INF)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MASTER;
        }

        if(!startWith(line, EXT_X_MEDIA_SEQUENCE) && startWith(line, EXT_X_MEDIA)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MASTER;
        }

        if(startWith(line, EXT_X_I_FRAME_STREAM_INF)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MASTER;
        }

        if(startWith(line, EXT_X_SESSION_DATA)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MASTER;
        }
        
        if(startWith(line, EXT_X_SESSION_KEY)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MASTER;
        }

        //parse media playlist
        if(M3UMedia::isMediaPlayListTag(line)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MEDIA;

            if(m3uMedia == NULL) {
                m3uMedia = new M3UMedia();
            }

            m3uMedia->parseMediaPlayList(line);

            continue;
        }

        //parse media m3u8
        if(isMediaSegmentTag(line)) {
            if(playListType == M3U8_TYPE_MASTER) {
                return ERROR_MALFORMED;
            }

            playListType = M3U8_TYPE_MEDIA;

            if(curMediaSegment == NULL) {
                curMediaSegment = new MediaSegment();
            }

            curMediaSegment->parseMediaSegemnt(line);
            continue;
        }

        if(!startWith(line, "#")) {
            if(playListType == M3U8_TYPE_MEDIA && curMediaSegment != NULL) {
                curMediaSegment->parseMediaSegemnt(line);
                mMediaSegmentVector.push_back(curMediaSegment);
                //curMediaSegment->dump();
                if(m3uMedia == NULL) {
                    m3uMedia = new M3UMedia();
                }

                if(curMediaSegment != NULL) {
                    m3uMedia->pushMediaSegment(*curMediaSegment);
                }

                delete curMediaSegment;
                curMediaSegment = NULL;
            }
        }
    }

    if((playListType == M3U8_TYPE_MEDIA) && (m3uMedia != NULL)) {
        // m3uMedia->setM3UVersion(m3u8Version);
        (*m3uMedia).dump();
    }

    std::cout<<"size is "<<mMediaSegmentVector.size()<<std::endl;

    return -1;
}

} // end namespace