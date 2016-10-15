#ifndef __RTSP_H__
#define __RTSP_H__

#include <string>


#define RTSP_EL "\r\n"
#define RTSP_VER "RTSP/1.0"
#define HDR_REQUIRE "Require"
#define HDR_ACCEPT "Accept"
#define PACKAGE "rtspd"
#define VERSION "1.0"
#define SDP_EL "\r\n"
#define HDR_TRANSPORT "Transport"
#define HDR_SESSION "Session"

namespace Rtsp {

int GetRtspCseg(const char *in, int &cseq);

void SetOptionsReply(int status, int cseq, std::string &response);

int SetDescribeReply(const char *in, int cseq, int &err, std::string &response);

} // end namespace

#endif


