#include <iostream>
#include <vector>
#include <list>

#include "Common.h"
#include "MediaSegment.h"
#include "M3u8Media.h"



namespace Hls {
	
M3UMedia::M3UMedia()
: sequence(0)
, bandWidth(0)
, hasEndTag(false)
, targetDuration(0) 
{
}

// #EXT-X-TARGETDURATION
int M3UMedia::getTargetDuration() 
{
    return targetDuration;
}

//#EXT-X-MEDIA-SEQUENCE
int M3UMedia::getMediaSequence() 
{
    return sequence;
}

// #EXT-X-ENDLIST
bool M3UMedia::hasEndList() 
{
    return hasEndTag;
}

bool M3UMedia::parseMediaPlayList(const std::string line) 
{
    if(startWith(line, EXT_X_TARGETDURATION)) {

        int colonPos = line.find(":");
        if(colonPos >= 0) {
            parseInt32(line.c_str() + colonPos + 1, &targetDuration);
        } else {
            targetDuration = 0;
        }

        std::cout<<"targetDuration is "<<targetDuration<<std::endl;

        return true;
    }

    if(startWith(line, EXT_X_MEDIA_SEQUENCE)) {
        int colonPos = line.find(":");
        if(colonPos >= 0) {
            parseInt32(line.c_str() + colonPos + 1, &sequence);
        } else {
            sequence = 0;
        }

        std::cout<<"sequence is "<<sequence<<std::endl;

        return true;
    }

    if(line == EXT_X_ENDLIST) {
        hasEndTag = true;
        std::cout<<"find endlist ..........."<<std::endl;

        return true;
    }

    return false;
}

bool M3UMedia::isMediaPlayListTag(std::string line) 
{
    static std::vector<std::string> tagsVector = std::vector<std::string>();

    if(tagsVector.size() <= 0) {
        tagsVector.push_back(EXT_X_TARGETDURATION);
        tagsVector.push_back(EXT_X_MEDIA_SEQUENCE);
        tagsVector.push_back(EXT_X_DISCONTINUITY_SEQUENCE);
        tagsVector.push_back(EXT_X_ENDLIST);
        tagsVector.push_back(EXT_X_PLAYLIST_TYPE);
        tagsVector.push_back(EXT_X_I_FRAMES_ONLY);
    }

    for(int i = 0; i < tagsVector.size(); i++) {
        if(startWith(line, tagsVector[i])) {
            return true;
        }
    }
    
    return false;
}

bool M3UMedia::pushMediaSegment(MediaSegment segment) 
{
    mediaSegments.push_back(segment);

    return true;
}

std::list<MediaSegment> M3UMedia::getSegmentList() 
{
    return mediaSegments;
}

void M3UMedia::dump() 
{
    std::cout<<"--------------- dump M3UMedia ----------------"<<std::endl;
    std::cout<<"targetDuration: "<<targetDuration<<std::endl;
    std::cout<<"mediaSequence: "<<sequence<<std::endl;
    std::cout<<"bandWidth: "<<bandWidth<<std::endl;
    std::cout<<"hasEndTag: "<<hasEndTag<<std::endl;

    for(std::list<MediaSegment>::iterator it = mediaSegments.begin(); it != mediaSegments.end(); ++it) {

    }

    std::cout<<"--------------- end dump M3UMedia ----------------"<<std::endl;
}

} // end namespace