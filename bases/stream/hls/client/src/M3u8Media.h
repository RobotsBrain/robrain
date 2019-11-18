#ifndef __M3U8MEDIA_H__
#define __M3U8MEDIA_H__

#include <string>
#include <list>



namespace Hls {

class M3UMedia
{
public:
    M3UMedia();
    ~M3UMedia();

    // #EXT-X-TARGETDURATION
    int getTargetDuration();

    //#EXT-X-MEDIA-SEQUENCE
    int getMediaSequence();

    // #EXT-X-ENDLIST
    bool hasEndList();

    bool parseMediaPlayList(const std::string line);

    static bool isMediaPlayListTag(std::string line);

    bool pushMediaSegment(MediaSegment segment);

    std::list<MediaSegment> getSegmentList();

    void dump();

private:
    int targetDuration;
    int sequence;
    int bandWidth;
    bool hasEndTag;
    std::list<MediaSegment> mediaSegments;
};

} // end namespace

#endif
