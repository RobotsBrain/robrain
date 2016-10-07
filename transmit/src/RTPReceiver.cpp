#include "RTPReceiver.h"



#if 0
void CRTPReceiver::InitBufferSize()  
{  
    m_ReceiveArray.SetMemberSize(BUFFER_SIZE);  
    m_pVideoData = new CVideoData();  
    memset(m_buffer, 0, BUFFER_SIZE);  
    m_current_size = 0;  
}  
  
void CRTPReceiver::OnRTCPCompoundPacket(RTCPCompoundPacket *pack,const RTPTime &receivetime,const RTPAddress *senderaddress)  
{  
    //std::cout<<"Got RTCP packet from: "<<senderaddress<<std::endl;  
}  

void CRTPReceiver::OnPollThreadStep()  
{  
    BeginDataAccess();  
          
    // check incoming packets  
    if (GotoFirstSourceWithData()) {  
        do {  
            RTPPacket *pack;  
            RTPSourceData *srcdat;  
              
            srcdat = GetCurrentSourceInfo();  
              
            while ((pack = GetNextPacket()) != NULL) {  
                ProcessRTPPacket(*srcdat, *pack);  
                DeletePacket(pack);  
            }  
        } while (GotoNextSourceWithData());  
    }  
          
    EndDataAccess();

    return;
}  
  
void CRTPReceiver::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)  
{  
    // You can inspect the packet and the source's info here  
    //std::cout<<"Packet type: "<<rtppack.GetPayloadType()<<std::endl;  
    //std::cout<<"Packet says: "<<(char *)rtppack.GetPayloadData()<<std::endl;  
    //test RTCP packet  
    /*int status = this->SendRTCPAPPPacket(0,(uint8_t*)&("123"),(void*)&("hel"),4); 
    checkerror(status);*/  
  
    if(rtppack.GetPayloadType() == H264) {  
        //std::cout<<"Got H264 packet：êo " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() <<std::endl;  
        if(rtppack.HasMarker()) {//如果是最后一包则进行组包  
            m_pVideoData->m_lLength = m_current_size + rtppack.GetPayloadLength();//得到数据包总的长度  
            memcpy(m_pVideoData->m_pBuffer, m_buffer, m_current_size);  
            memcpy(m_pVideoData->m_pBuffer + m_current_size ,rtppack.GetPayloadData(),rtppack.GetPayloadLength());  
              
            m_ReceiveArray.Add(m_pVideoData);//添加到接收队列  
  
            memset(m_buffer,0,m_current_size);//清空缓存，为下次做准备  
            m_current_size = 0;  
        } else {  //放入缓冲区，在此必须确保有序
            unsigned char* p = rtppack.GetPayloadData();  

            memcpy(m_buffer + m_current_size,rtppack.GetPayloadData(),rtppack.GetPayloadLength());  
            m_current_size += rtppack.GetPayloadLength();  
        }  
    }

    return; 
}

void StartReceive()  
{  
    /*CRTPReceiver sess;*/  
    sess.InitBufferSize();  
    std::string ipstr;  
    int status;  
  
    // Now, we'll create a RTP session, set the destination  
    // and poll for incoming data.  
    RTPUDPv4TransmissionParams transparams;  
    RTPSessionParams sessparams;  
      
    // IMPORTANT: The local timestamp unit MUST be set, otherwise  
    //            RTCP Sender Report info will be calculated wrong  
    // In this case, we'll be just use 9000 samples per second.  
    sessparams.SetOwnTimestampUnit(1.0/9000.0);       
      
    transparams.SetPortbase(PORT_BASE);  
    status = sess.Create(sessparams,&transparams);    
    checkerror(status);  
  
    uint32_t dst_ip = inet_addr(DST_IP);  
    dst_ip = ntohl(dst_ip);  
    RTPIPv4Address addr(dst_ip,DST_PORT);  
    status = sess.AddDestination(addr);  
      
    checkerror(status);  
}  

#endif