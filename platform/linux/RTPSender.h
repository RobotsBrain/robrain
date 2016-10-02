#ifndef __RTPSENDER_H__
#define __RTPSENDER_H__

#include "rtcpapppacket.h"
#include "rtptimeutilities.h"
#include "rtpaddress.h"
#include "rtpsourcedata.h"



class CRTPSender : public RTPSession  
{  
public:  
    CRTPSender(void);  
    ~CRTPSender(void);  
  
protected:  
    void OnAPPPacket(RTCPAPPPacket *apppacket, const RTPTime &receivetime, const RTPAddress *senderaddress);  
    void OnBYEPacket(RTPSourceData *srcdat);
    void OnBYETimeout(RTPSourceData *srcdat);

public:  
    void SendH264Nalu(unsigned char *h264Buf, int buflen);  
    void SetParamsForSendingH264();  
};  

#endif