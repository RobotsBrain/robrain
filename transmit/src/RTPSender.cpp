#include <stdio.h>
#include <string>

#include "rtcpapppacket.h"
#include "rtpsourcedata.h"
#include "rtperrors.h"
#include "rtpipv4address.h"

#include "RTPSender.h"


#define MAX_RTP_PKT_LENGTH  1360  
#define H264                96
#define SSRC                100
#define DEST_PORT           1234  
#define BASE_PORT           2222

bool CheckError(int rtperr)  
{  
    if (rtperr < 0) {  
        printf("error: %s\n", RTPGetErrorString(rtperr).c_str());  
        return false;  
    }

    return true;  
}  
  
/**********************************************************************************************/
  
CRTPSender::CRTPSender(void)  
{  
}  

CRTPSender::~CRTPSender(void)  
{  
}  
  
void CRTPSender::Start()
{
    int status;
    std::string destip_str = "192.168.0.105";  
    uint32_t destip = inet_addr(destip_str.c_str());  

    //RTP+RTCP库初始化SOCKET环境
    m_sessparams.SetOwnTimestampUnit(1.0/9000.0); //时间戳单位  
    m_sessparams.SetAcceptOwnPackets(true);   //接收自己发送的数据包  
    m_sessparams.SetUsePredefinedSSRC(true);  //设置使用预先定义的SSRC  
    m_sessparams.SetPredefinedSSRC(SSRC);     //定义SSRC  

    m_transparams.SetPortbase(BASE_PORT);  

    status = this->Create(m_sessparams, &m_transparams);
    CheckError(status);  
  
    destip = ntohl(destip);  
    RTPIPv4Address addr(destip, DEST_PORT);  
    status = this->AddDestination(addr);  
    CheckError(status);

    this->SetDefaultPayloadType(96);
    this->SetDefaultMark(false);
    this->SetDefaultTimestampIncrement(160);

    return;
}

void CRTPSender::Stop()
{
    return;
}

void CRTPSender::OnAPPPacket(RTCPAPPPacket *apppacket,const RTPTime &receivetime,const RTPAddress *senderaddress)  
{//收到RTCP APP数据  
    // std::cout<<"Got RTCP packet from: "<<senderaddress<<std::endl;  
    // std::cout<<"Got RTCP subtype: "<<apppacket->GetSubType()<<std::endl;  
    // std::cout<<"Got RTCP data: "<<(char *)apppacket->GetAPPData()<<std::endl;

    return ;  
}  
  
void CRTPSender::SendH264Nalu(unsigned char *h264Buf, int buflen)   
{  
    unsigned char *pSendbuf = h264Buf; //发送数据指针  
      
    //去除前导码0x000001或者0x00000001  
    // if(0x01 == h264Buf[2]) {  
    //     pSendbuf = &h264Buf[3];  
    //     buflen -= 3;  
    // } else {  
    //     pSendbuf = &h264Buf[4];  
    //     buflen -= 4;  
    // }

    char sendbuf[1430];   //发送的数据缓冲

    memset(sendbuf, 0, 1430);  
  
    int status;    
  
    printf("send packet length %d \n", buflen);  
  
    if(buflen <= MAX_RTP_PKT_LENGTH) {    
        memcpy(sendbuf, pSendbuf, buflen);    
        status = this->SendPacket((void *)sendbuf,buflen);  
     
        CheckError(status);
    } else if(buflen > MAX_RTP_PKT_LENGTH) {  
        //设置标志位Mark为0  
        this->SetDefaultMark(false);  
        //printf("buflen = %d\n",buflen);  
        //得到该需要用多少长度为MAX_RTP_PKT_LENGTH字节的RTP包来发送  
        int k = 0, l = 0;    
        k = buflen / MAX_RTP_PKT_LENGTH;  
        l = buflen % MAX_RTP_PKT_LENGTH;  
        int t = 0; //指示当前发送的是第几个分片RTP包  
  
        char nalHeader = pSendbuf[0]; // NALU 头ª¡¤  
        while(t < k || (t == k && l > 0)) {    
            if((0 == t) || (t < k && 0 != t)) { //第一包到最后包的前一包  
              
                /*sendbuf[0] = (nalHeader & 0x60)|28;   
                sendbuf[1] = (nalHeader & 0x1f); 
                if ( 0 == t ) { 
                    sendbuf[1] |= 0x80; 
                } 
                memcpy(sendbuf+2,&pSendbuf[t*MAX_RTP_PKT_LENGTH],MAX_RTP_PKT_LENGTH); 
                status = this->SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2);*/  
                memcpy(sendbuf, &pSendbuf[t * MAX_RTP_PKT_LENGTH], MAX_RTP_PKT_LENGTH);  
                status = this->SendPacket((void *)sendbuf, MAX_RTP_PKT_LENGTH);  
                CheckError(status);  
                t++;  
            } else if((k == t && l > 0) || (t == (k-1) && l == 0)) {  //最后一包
                //设置标志位Mark为1  
                this->SetDefaultMark(true);  
  
                int iSendLen;  
                if(l > 0) {  
                    iSendLen = buflen - t*MAX_RTP_PKT_LENGTH;  
                } else {
                    iSendLen = MAX_RTP_PKT_LENGTH;
                } 

                //sendbuf[0] = (nalHeader & 0x60)|28;    
                //sendbuf[1] = (nalHeader & 0x1f);  
                //sendbuf[1] |= 0x40;  
  
                //memcpy(sendbuf+2,&pSendbuf[t*MAX_RTP_PKT_LENGTH],iSendLen);  
                //status = this->SendPacket((void *)sendbuf,iSendLen+2);  
     
                memcpy(sendbuf, &pSendbuf[t*MAX_RTP_PKT_LENGTH], iSendLen);  
                status = this->SendPacket((void *)sendbuf, iSendLen);  
  
                CheckError(status);  
                t++;  
            }  
        }  
    }

    return;
}
  
void CRTPSender::SetParamsForSendingH264()  
{  
    this->SetDefaultPayloadType(H264);//设置传输类型  
    this->SetDefaultMark(true);      //设置位  
    this->SetTimestampUnit(1.0/9000.0); //设置采样间隔  
    this->SetDefaultTimestampIncrement(3600);//设置时间戳增加间隔

    return;
}  
  
void CRTPSender::OnBYEPacket(RTPSourceData *srcdat)  
{ 
}  
  
void CRTPSender::OnBYETimeout(RTPSourceData *srcdat)  
{
}
  

