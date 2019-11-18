#ifndef __HLSCOMMON_H__
#define __HLSCOMMON_H__

#include <string>
#include <map>



namespace Hls {

//Basic Tags
#define EXTM3U  						"#EXTM3U"
#define EXT_X_VERSION  					"#EXT-X-VERSION"

//Media Playlist Tags
#define EXT_X_TARGETDURATION  			"#EXT-X-TARGETDURATION"
#define EXT_X_MEDIA_SEQUENCE  			"#EXT-X-MEDIA-SEQUENCE"
#define EXT_X_DISCONTINUITY_SEQUENCE  	"#EXT-X-DISCONTINUITY-SEQUENCE"
#define EXT_X_ENDLIST  					"#EXT-X-ENDLIST"
#define EXT_X_PLAYLIST_TYPE  			"#EXT-X-PLAYLIST-TYPE"
#define EXT_X_I_FRAMES_ONLY  			"#EXT-X-I-FRAMES-ONLY"

//Master Playlist Tags
#define EXT_X_MEDIA  					"#EXT-X-MEDIA"
#define EXT_X_STREAM_INF  				"#EXT-X-STREAM-INF"
#define EXT_X_I_FRAME_STREAM_INF  		"#EXT-X-I-FRAME-STREAM-INF"
#define EXT_X_SESSION_DATA  			"#EXT-X-SESSION-DATA"
#define EXT_X_SESSION_KEY  				"#EXT-X-SESSION-KEY"

//Media or Master Playlist Tags
#define EXT_X_INDEPENDENT_SEGMENTS  	"#EXT-X-INDEPENDENT-SEGMENTS"
#define EXT_X_START  					"#EXT-X-START"

//Media Segment Tags
#define EXTINF  						"#EXTINF"
#define EXT_X_BYTERANGE  				"#EXT-X-BYTERANGE"
#define EXT_X_DISCONTINUITY  			"#EXT-X-DISCONTINUITY"
#define EXT_X_KEY  						"#EXT-X-KEY"
#define EXT_X_MAP  						"#EXT-X-MAP"
#define EXT_X_PROGRAM_DATE_TIME  		"#EXT-X-PROGRAM-DATE-TIME"
#define EXT_X_DATERANGE  				"#EXT-X-DATERANGE"

enum {
    ERROR_UNKNOWN = 0,
    ERROR_NOT_M3U8_FILE = 1,
    ERROR_MALFORMED = 2,
};

bool startWith(const std::string& str, const std::string& subStr);

std::string getLine(const char* data, int *offsetCur, int size);

std::map<std::string, std::string>* parseAttr(std::string line);

bool parseDouble(const char* s, double *x);

bool parseInt32(const char* s, int *x);

} // end namespace

#endif
