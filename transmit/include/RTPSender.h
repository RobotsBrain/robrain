#ifndef __RTPSENDER_H__
#define __RTPSENDER_H__

#include "rtcpapppacket.h"
#include "rtptimeutilities.h"
#include "rtpaddress.h"
#include "rtpsourcedata.h"
#include "rtpsession.h"


using namespace jrtplib;

class CRTPSender: public RTPSession
{  
public:  
    CRTPSender();  
    ~CRTPSender();  
  
protected:  
    void OnAPPPacket(RTCPAPPPacket *apppacket, const RTPTime &receivetime, const RTPAddress *senderaddress);  
    void OnBYEPacket(RTPSourceData *srcdat);
    void OnBYETimeout(RTPSourceData *srcdat);

public:  
    void SendH264Nalu(unsigned char *h264Buf, int buflen);  
    void SetParamsForSendingH264();  
};  

#endif