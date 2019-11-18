#ifndef __MEDIASEGMENT_H__
#define __MEDIASEGMENT_H__

#include <string>



namespace Hls {

class MediaSegment {
public:
    MediaSegment();
    ~MediaSegment();

    int parseMediaSegemnt(const std::string &line);

    std::string getUri();

    int getDuration();

private:
    std::string m_uri;
    bool        m_discontinuity;
    double      m_duration;
};

}  // end namespace

#endif
