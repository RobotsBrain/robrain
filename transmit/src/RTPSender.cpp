#include <stdio.h>

#include "rtcpapppacket.h"
#include "rtpsourcedata.h"
#include "rtperrors.h"

#include "RTPSender.h"


#define MAX_RTP_PKT_LENGTH 1360  
#define H264               96



bool CheckError(int rtperr)  
{  
    if (rtperr < 0) {  
        printf("error: %s\n", RTPGetErrorString(rtperr).c_str());  
        return false;  
    }

    return true;  
}  
  
  
CRTPSender::CRTPSender(void)  
{  
}  
  
  
CRTPSender::~CRTPSender(void)  
{  
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
  
#if 0
#define SSRC           100
#define DEST_PORT     1234  
#define BASE_PORT     2222  
  
int iNal   = 0;  
x264_nal_t* pNals = NULL;  
  
  
void SetRTPParams(CRTPSender& sess,uint32_t destip,uint16_t destport,uint16_t baseport)  
{  
    int status;    
    //RTP+RTCP库初始化SOCKET环境  
    RTPUDPv4TransmissionParams transparams;  
    RTPSessionParams sessparams;  
    sessparams.SetOwnTimestampUnit(1.0/9000.0); //时间戳单位  
    sessparams.SetAcceptOwnPackets(true);   //接收自己发送的数据包  
    sessparams.SetUsePredefinedSSRC(true);  //设置使用预先定义的SSRC  
    sessparams.SetPredefinedSSRC(SSRC);     //定义SSRC  
     
    transparams.SetPortbase(baseport);  
  
    status = sess.Create(sessparams,&transparams);    
    CheckError(status);  
  
    destip = ntohl(destip);  
    RTPIPv4Address addr(destip,destport);  
    status = sess.AddDestination(addr);  
    CheckError(status);  
      
    //为发送H264包设置参数  
    //sess.SetParamsForSendingH264();
}  

bool InitSocket()  
{  
    int Error;  
    WORD VersionRequested;  
    WSADATA WsaData;  
    VersionRequested=MAKEWORD(2,2);  
    Error=WSAStartup(VersionRequested,&WsaData); //启动WinSock2  
    if(Error!=0) {  
        printf("Error:Start WinSock failed!\n");  
        return false;  
    } else {  
        if(LOBYTE(WsaData.wVersion)!=2||HIBYTE(WsaData.wHighVersion)!=2) {  
            printf("Error:The version is WinSock2!\n");  
            WSACleanup();  
            return false;  
        }
    }

    return true;  
}  
  
void CloseSocket(CRTPSender sess)  
{  
    //发送一个BYE包离开会话最多等待秒钟超时则不发送  
    sess.BYEDestroy(RTPTime(3,0),0,0);  
    WSACleanup();  
}  
  
int main(int argc, char** argv)  
{  
    InitSocket();  
    CRTPSender sender;  
    string destip_str = "127.0.0.1";  
    uint32_t dest_ip = inet_addr(destip_str.c_str());             
  
    SetRTPParams(sender,dest_ip,DEST_PORT,BASE_PORT);  
    sender.SetParamsForSendingH264();  
  
    //…x264设置参数等步骤，具体参见上篇博客  
    for(int i = 0; i < nFrames ; i++ ) {  
        //读取一帧  
        read_frame_y4m(pPicIn,(hnd_t*)y4m_hnd,i);  
        if( i ==0 )  
            pPicIn->i_pts = i;  
        else  
            pPicIn->i_pts = i - 1;  
          
        //编码  
        int frame_size = x264_encoder_encode(pX264Handle,&pNals,&iNal,pPicIn,pPicOut);  
  
        if(frame_size >0) {
            for (int i = 0; i < iNal; ++i) {//将编码数据写入文件t  
                //fwrite(pNals[i].p_payload, 1, pNals[i].i_payload, pFile);  
                //发送编码文件  
                sender.SendH264Nalu(pNals[i].p_payload,pNals[i].i_payload);  
                RTPTime::Wait(RTPTime(1,0));  
            }  
        }  
    }  
  
    CloseSocket(sender);  
    //一些清理工作…  
}

#endif


