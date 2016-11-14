#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "Ts.h"
#include "Mux.h"



unsigned char m_One_Frame_Buf[MAX_ONE_FRAME_SIZE];
TsPes m_video_tspes;
TsPes m_audio_tspes;
unsigned int WritePacketNum = 0;
unsigned long  Timestamp_video = 0;    //一帧视频所用时间
unsigned long  Timestamp_audio = 0;    //一帧音频所用时间 
STREAMCALLBACK m_cbk = NULL;

void SetCallback(STREAMCALLBACK pcbk)
{
	m_cbk = pcbk;
}

int WriteAdaptive_flags_Head(Ts_Adaptation_field  * ts_adaptation_field,unsigned int Videopts)
{
	//填写自适应段
	ts_adaptation_field->discontinuty_indicator = 0;
	ts_adaptation_field->random_access_indicator = 0;
	ts_adaptation_field->elementary_stream_priority_indicator = 0;
	ts_adaptation_field->PCR_flag = 1;                                          //只用到这个
	ts_adaptation_field->OPCR_flag = 0;
	ts_adaptation_field->splicing_point_flag = 0;
	ts_adaptation_field->transport_private_data_flag = 0;
	ts_adaptation_field->adaptation_field_extension_flag = 0;

	//需要自己算
	ts_adaptation_field->pcr = Videopts * 300;
	ts_adaptation_field->adaptation_field_length = 7;                          //占用7位

	ts_adaptation_field->opcr = 0;
	ts_adaptation_field->splice_countdown = 0;
	ts_adaptation_field->private_data_len = 0;

	return 1;
}

int WriteAdaptive_flags_Tail(Ts_Adaptation_field  * ts_adaptation_field)
{
	//填写自适应段
	ts_adaptation_field->discontinuty_indicator = 0;
	ts_adaptation_field->random_access_indicator = 0;
	ts_adaptation_field->elementary_stream_priority_indicator = 0;
	ts_adaptation_field->PCR_flag = 0;                                          //只用到这个
	ts_adaptation_field->OPCR_flag = 0;
	ts_adaptation_field->splicing_point_flag = 0;
	ts_adaptation_field->transport_private_data_flag = 0;
	ts_adaptation_field->adaptation_field_extension_flag = 0;

	//需要自己算
	ts_adaptation_field->pcr  = 0;
	ts_adaptation_field->adaptation_field_length = 1;                          //占用1位标志所用的位

	ts_adaptation_field->opcr = 0;
	ts_adaptation_field->splice_countdown = 0;
	ts_adaptation_field->private_data_len = 0;

	return 1;
}

int CreateAdaptive_Ts(Ts_Adaptation_field * ts_adaptation_field,unsigned char * buf,unsigned int AdaptiveLength)
{
	unsigned int CurrentAdaptiveLength = 1;                                 //当前已经用的自适应段长度  
	unsigned char Adaptiveflags = 0;                                        //自适应段的标志
	unsigned int adaptive_pos = 0;

	//填写自适应字段
	if (ts_adaptation_field->adaptation_field_length > 0) {
		adaptive_pos += 1;                                                  //自适应段的一些标志所占用的1个字节
		CurrentAdaptiveLength += 1;

		if (ts_adaptation_field->discontinuty_indicator) {
			Adaptiveflags |= 0x80;
		}

		if (ts_adaptation_field->random_access_indicator) {
			Adaptiveflags |= 0x40;
		}

		if (ts_adaptation_field->elementary_stream_priority_indicator) {
			Adaptiveflags |= 0x20;
		}

		if (ts_adaptation_field->PCR_flag) {
			unsigned long long pcr_base;
			unsigned int pcr_ext;

			pcr_base = (ts_adaptation_field->pcr / 300);
			pcr_ext = (ts_adaptation_field->pcr % 300);

			Adaptiveflags |= 0x10;

			buf[adaptive_pos + 0] = (pcr_base >> 25) & 0xff;
			buf[adaptive_pos + 1] = (pcr_base >> 17) & 0xff;
			buf[adaptive_pos + 2] = (pcr_base >> 9) & 0xff;
			buf[adaptive_pos + 3] = (pcr_base >> 1) & 0xff;
			buf[adaptive_pos + 4] = pcr_base << 7 | pcr_ext >> 8 | 0x7e;
			buf[adaptive_pos + 5] = (pcr_ext) & 0xff;
			adaptive_pos += 6;

			CurrentAdaptiveLength += 6;
		}

		if (ts_adaptation_field->OPCR_flag) {
			unsigned long long opcr_base;
			unsigned int opcr_ext;

			opcr_base = (ts_adaptation_field->opcr / 300);
			opcr_ext = (ts_adaptation_field->opcr % 300);

			Adaptiveflags |= 0x08;

			buf[adaptive_pos + 0] = (opcr_base >> 25) & 0xff;
			buf[adaptive_pos + 1] = (opcr_base >> 17) & 0xff;
			buf[adaptive_pos + 2] = (opcr_base >> 9) & 0xff;
			buf[adaptive_pos + 3] = (opcr_base >> 1) & 0xff;
			buf[adaptive_pos + 4] = ((opcr_base << 7) & 0x80) | ((opcr_ext >> 8) & 0x01);
			buf[adaptive_pos + 5] = (opcr_ext) & 0xff;
			adaptive_pos += 6;
			CurrentAdaptiveLength += 6;
		}

		if (ts_adaptation_field->splicing_point_flag) {
			buf[adaptive_pos] = ts_adaptation_field->splice_countdown;

			Adaptiveflags |= 0x04;

			adaptive_pos += 1;
			CurrentAdaptiveLength += 1;
		}

		if (ts_adaptation_field->private_data_len > 0) {
			Adaptiveflags |= 0x02;
			if (1+ ts_adaptation_field->private_data_len > AdaptiveLength - CurrentAdaptiveLength) {
				printf("private_data_len error !\n");
				return getchar();
			} else {
				buf[adaptive_pos] = ts_adaptation_field->private_data_len;
				adaptive_pos += 1;
				memcpy (buf + adaptive_pos, ts_adaptation_field->private_data, ts_adaptation_field->private_data_len);
				adaptive_pos += ts_adaptation_field->private_data_len;

				CurrentAdaptiveLength += (1 + ts_adaptation_field->private_data_len) ;
			}
		}

		if (ts_adaptation_field->adaptation_field_extension_flag) {
			Adaptiveflags |= 0x01;
			buf[adaptive_pos + 1] = 1;
			buf[adaptive_pos + 2] = 0;
			CurrentAdaptiveLength += 2;
		}

		buf[0] = Adaptiveflags;                        //将标志放入内存
	}

	return 1;
}

int PES2TS(TsPes *ts_pes, unsigned int avPid, Ts_Adaptation_field *ts_adaptation_field_Head, Ts_Adaptation_field *ts_adaptation_field_Tail,
		   unsigned long  Videopts, unsigned long Adudiopts)
{
	TsPacketHeader ts_header;
	unsigned int ts_pos = 0;
	unsigned int FirstPacketLoadLength = 0 ;                                   //分片包的第一个包的负载长度
	unsigned int NeafPacketCount = 0;                                          //分片包的个数
	unsigned int AdaptiveLength = 0;                                           //要填写0XFF的长度
	unsigned char *NeafBuf = NULL;                                            //分片包 总负载的指针
	unsigned char TSbuf[TS_PACKET_SIZE];

	memset(TSbuf, 0, TS_PACKET_SIZE); 
	FirstPacketLoadLength = 188 - 4 - 1 - ts_adaptation_field_Head->adaptation_field_length - 14; //计算分片包的第一个包的负载长度
	NeafPacketCount += 1;                                                                   //第一个分片包  

	//一个包的情况
	if (ts_pes->Pes_Packet_Length_Beyond < FirstPacketLoadLength) {                         //这里是 sps ，pps ，sei等
		memset(TSbuf, 0xFF, TS_PACKET_SIZE);

		WriteStruct_Packetheader(TSbuf, avPid, 0x01, 0x03);                          //PID = TS_H264_PID,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x03,含有调整字段和有效负载 ；
		
		ts_pos += 4;
		TSbuf[ts_pos + 0] = 184 - ts_pes->Pes_Packet_Length_Beyond - 9 - 5 - 1 ;
		TSbuf[ts_pos + 1] = 0x00;
		ts_pos += 2; 
		memset(TSbuf + ts_pos,0xFF,(184 - ts_pes->Pes_Packet_Length_Beyond - 9 - 5 - 2));
		ts_pos += (184 - ts_pes->Pes_Packet_Length_Beyond - 9 - 5 - 2);

		TSbuf[ts_pos + 0] = (ts_pes->packet_start_code_prefix >> 16) & 0xFF;
		TSbuf[ts_pos + 1] = (ts_pes->packet_start_code_prefix >> 8) & 0xFF; 
		TSbuf[ts_pos + 2] = ts_pes->packet_start_code_prefix & 0xFF;
		TSbuf[ts_pos + 3] = ts_pes->stream_id;
		TSbuf[ts_pos + 4] = ((ts_pes->PES_packet_length) >> 8) & 0xFF;
		TSbuf[ts_pos + 5] = (ts_pes->PES_packet_length) & 0xFF;
		TSbuf[ts_pos + 6] = ts_pes->marker_bit << 6 | ts_pes->PES_scrambling_control << 4 | ts_pes->PES_priority << 3 |
			ts_pes->data_alignment_indicator << 2 | ts_pes->copyright << 1 |ts_pes->original_or_copy;
		TSbuf[ts_pos + 7] = ts_pes->PTS_DTS_flags << 6 |ts_pes->ESCR_flag << 5 | ts_pes->ES_rate_flag << 4 |
			ts_pes->DSM_trick_mode_flag << 3 | ts_pes->additional_copy_info_flag << 2 | ts_pes->PES_CRC_flag << 1 | ts_pes->PES_extension_flag;
		TSbuf[ts_pos + 8] = ts_pes->PES_header_data_length;
		ts_pos += 9;

		if (ts_pes->stream_id == TS_H264_STREAM_ID) {
			TSbuf[ts_pos + 0] = (((0x3 << 4) | ((Videopts>> 29) & 0x0E) | 0x01) & 0xff);
			TSbuf[ts_pos + 1]= (((((Videopts >> 14) & 0xfffe) | 0x01) >> 8) & 0xff);
			TSbuf[ts_pos + 2]= ((((Videopts >> 14) & 0xfffe) | 0x01) & 0xff);
			TSbuf[ts_pos + 3]= (((((Videopts << 1) & 0xfffe) | 0x01) >> 8) & 0xff);
			TSbuf[ts_pos + 4]= ((((Videopts << 1) & 0xfffe) | 0x01) & 0xff);
			ts_pos += 5;
		} else if (ts_pes->stream_id == TS_AAC_STREAM_ID) {
			TSbuf[ts_pos + 0] = (((0x3 << 4) | ((Adudiopts>> 29) & 0x0E) | 0x01) & 0xff);
			TSbuf[ts_pos + 1]= (((((Adudiopts >> 14) & 0xfffe) | 0x01) >> 8) & 0xff);
			TSbuf[ts_pos + 2]= ((((Adudiopts >> 14) & 0xfffe) | 0x01) & 0xff);
			TSbuf[ts_pos + 3]= (((((Adudiopts << 1) & 0xfffe) | 0x01) >> 8) & 0xff);
			TSbuf[ts_pos + 4]= ((((Adudiopts << 1) & 0xfffe) | 0x01) & 0xff);
			ts_pos += 5;
		} else {
			printf("ts_pes->stream_id  error 0x%x \n",ts_pes->stream_id);
			return getchar();
		}
		memcpy(TSbuf + ts_pos,ts_pes->Es,ts_pes->Pes_Packet_Length_Beyond);  

		//将包写入文件
		m_cbk(TSbuf, TS_PACKET_SIZE);

		WritePacketNum ++;                                                      //已经写入文件的包个数++
		
		return WritePacketNum;
	}
	
	NeafPacketCount += (ts_pes->Pes_Packet_Length_Beyond - FirstPacketLoadLength)/ 184;     
	NeafPacketCount += 1;                                                                   //最后一个分片包
	AdaptiveLength = 188 - 4 - 1 - ((ts_pes->Pes_Packet_Length_Beyond - FirstPacketLoadLength)% 184)  ;  //要填写0XFF的长度
	if ((WritePacketNum % 40) == 0) {                                                        //每40个包打一个 pat,一个pmt
		WriteStruct_Pat(m_One_Frame_Buf);
		m_cbk(m_One_Frame_Buf, TS_PACKET_SIZE);

		WriteStruct_Pmt(m_One_Frame_Buf);
		m_cbk(m_One_Frame_Buf, TS_PACKET_SIZE);
	}
	//开始处理第一个包,分片包的个数最少也会是两个 
	WriteStruct_Packetheader(TSbuf, avPid, 0x01, 0x03);                              //PID = TS_H264_PID,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x03,含有调整字段和有效负载 ；
	ts_pos += 4;
	TSbuf[ts_pos] = ts_adaptation_field_Head->adaptation_field_length;                      //自适应字段的长度，自己填写的
	ts_pos += 1;                                                       

	CreateAdaptive_Ts(ts_adaptation_field_Head, TSbuf + ts_pos, (188 - 4 - 1 - 14));          //填写自适应字段
	ts_pos += ts_adaptation_field_Head->adaptation_field_length;                            //填写自适应段所需要的长度

	TSbuf[ts_pos + 0] = (ts_pes->packet_start_code_prefix >> 16) & 0xFF;
	TSbuf[ts_pos + 1] = (ts_pes->packet_start_code_prefix >> 8) & 0xFF; 
	TSbuf[ts_pos + 2] = ts_pes->packet_start_code_prefix & 0xFF;
	TSbuf[ts_pos + 3] = ts_pes->stream_id;
	TSbuf[ts_pos + 4] = ((ts_pes->PES_packet_length) >> 8) & 0xFF;
	TSbuf[ts_pos + 5] = (ts_pes->PES_packet_length) & 0xFF;
	TSbuf[ts_pos + 6] = ts_pes->marker_bit << 6 | ts_pes->PES_scrambling_control << 4 | ts_pes->PES_priority << 3 |
		ts_pes->data_alignment_indicator << 2 | ts_pes->copyright << 1 |ts_pes->original_or_copy;
	TSbuf[ts_pos + 7] = ts_pes->PTS_DTS_flags << 6 |ts_pes->ESCR_flag << 5 | ts_pes->ES_rate_flag << 4 |
		ts_pes->DSM_trick_mode_flag << 3 | ts_pes->additional_copy_info_flag << 2 | ts_pes->PES_CRC_flag << 1 | ts_pes->PES_extension_flag;
	TSbuf[ts_pos + 8] = ts_pes->PES_header_data_length;
	ts_pos += 9;

	if (ts_pes->stream_id == TS_H264_STREAM_ID) {
		TSbuf[ts_pos + 0] = (((0x3 << 4) | ((Videopts>> 29) & 0x0E) | 0x01) & 0xff);
		TSbuf[ts_pos + 1]= (((((Videopts >> 14) & 0xfffe) | 0x01) >> 8) & 0xff);
		TSbuf[ts_pos + 2]= ((((Videopts >> 14) & 0xfffe) | 0x01) & 0xff);
		TSbuf[ts_pos + 3]= (((((Videopts << 1) & 0xfffe) | 0x01) >> 8) & 0xff);
		TSbuf[ts_pos + 4]= ((((Videopts << 1) & 0xfffe) | 0x01) & 0xff);
		ts_pos += 5;
	} else if (ts_pes->stream_id == TS_AAC_STREAM_ID) {
		TSbuf[ts_pos + 0] = (((0x3 << 4) | ((Adudiopts>> 29) & 0x0E) | 0x01) & 0xff);
		TSbuf[ts_pos + 1]= (((((Adudiopts >> 14) & 0xfffe) | 0x01) >> 8) & 0xff);
		TSbuf[ts_pos + 2]= ((((Adudiopts >> 14) & 0xfffe) | 0x01) & 0xff);
		TSbuf[ts_pos + 3]= (((((Adudiopts << 1) & 0xfffe) | 0x01) >> 8) & 0xff);
		TSbuf[ts_pos + 4]= ((((Adudiopts << 1) & 0xfffe) | 0x01) & 0xff);
		ts_pos += 5;
	} else {
		printf("ts_pes->stream_id  error 0x%x \n",ts_pes->stream_id);
		return getchar();
	}

	NeafBuf = ts_pes->Es ;
	memcpy(TSbuf + ts_pos,NeafBuf,FirstPacketLoadLength);  

	NeafBuf += FirstPacketLoadLength;
	ts_pes->Pes_Packet_Length_Beyond -= FirstPacketLoadLength;
	//将包写入文件
	m_cbk(TSbuf, TS_PACKET_SIZE);

	WritePacketNum ++;                                                      //已经写入文件的包个数++

	while(ts_pes->Pes_Packet_Length_Beyond) {
		ts_pos = 0;
		memset(TSbuf, 0, TS_PACKET_SIZE); 

		if ((WritePacketNum % 40) == 0) {                                                     //每40个包打一个 pat,一个pmt
			WriteStruct_Pat(m_One_Frame_Buf);
			m_cbk(m_One_Frame_Buf, TS_PACKET_SIZE);

			WriteStruct_Pmt(m_One_Frame_Buf);
			m_cbk(m_One_Frame_Buf, TS_PACKET_SIZE);
		}

		if(ts_pes->Pes_Packet_Length_Beyond >= 184) {
			//处理中间包   
			WriteStruct_Packetheader(TSbuf, avPid, 0x00, 0x01);     //PID = TS_H264_PID,不是有效荷载单元起始指示符_play_init = 0x00, ada_field_C,0x01,仅有有效负载；    
			ts_pos += 4;
            memcpy(TSbuf + ts_pos,NeafBuf,184); 
			NeafBuf += 184;
			ts_pes->Pes_Packet_Length_Beyond -= 184;
			m_cbk(TSbuf, TS_PACKET_SIZE);
		} else {
			if(ts_pes->Pes_Packet_Length_Beyond == 183 || ts_pes->Pes_Packet_Length_Beyond == 182) {
				WriteStruct_Packetheader(TSbuf, avPid, 0x00, 0x03);   //PID = TS_H264_PID,不是有效荷载单元起始指示符_play_init = 0x00, ada_field_C,0x03,含有调整字段和有效负载；
				ts_pos += 4;
				TSbuf[ts_pos + 0] = 0x01;
				TSbuf[ts_pos + 1] = 0x00;
				ts_pos += 2;
				memcpy(TSbuf + ts_pos,NeafBuf,182); 
				  
				NeafBuf += 182;
				ts_pes->Pes_Packet_Length_Beyond -= 182;
			} else {
				WriteStruct_Packetheader(TSbuf, avPid, 0x00, 0x03);  //PID = TS_H264_PID,不是有效荷载单元起始指示符_play_init = 0x00, ada_field_C,0x03,含有调整字段和有效负载；
				ts_pos += 4;
				TSbuf[ts_pos + 0] = 184 - ts_pes->Pes_Packet_Length_Beyond - 1 ;
				TSbuf[ts_pos + 1] = 0x00;
				ts_pos += 2;
				memset(TSbuf + ts_pos, 0xFF, (184 - ts_pes->Pes_Packet_Length_Beyond - 2)); 
				ts_pos += (184 - ts_pes->Pes_Packet_Length_Beyond - 2);
				memcpy(TSbuf + ts_pos, NeafBuf, ts_pes->Pes_Packet_Length_Beyond);
				ts_pes->Pes_Packet_Length_Beyond = 0;
			}

			m_cbk(TSbuf, TS_PACKET_SIZE);
		}

		WritePacketNum ++;  
	}

	return WritePacketNum ;
}

int AAC2PES(TsPes *tsaacpes, unsigned long Adudiopts, unsigned char *pData, int iDataSize)
{
	unsigned int OneFrameLen_AAC = 0;

	//读取出一帧数据
	OneFrameLen_AAC = iDataSize;
	memcpy(tsaacpes->Es, pData, iDataSize);

	tsaacpes->packet_start_code_prefix = 0x000001;
	tsaacpes->stream_id = TS_AAC_STREAM_ID;                                //E0~EF表示是视频的,C0~DF是音频,H264-- E0
	tsaacpes->PES_packet_length = 0; // OneFrameLen_AAC + 8 ;             //一帧数据的长度 不包含 PES包头 ,8自适应段的长度
	tsaacpes->Pes_Packet_Length_Beyond = OneFrameLen_AAC;                  //= OneFrameLen_aac;     //这里读错了一帧  
	tsaacpes->marker_bit = 0x02;
	tsaacpes->PES_scrambling_control = 0x00;                               //人选字段 存在，不加扰
	tsaacpes->PES_priority = 0x00;
	tsaacpes->data_alignment_indicator = 0x00;
	tsaacpes->copyright = 0x00;
	tsaacpes->original_or_copy = 0x00;
	tsaacpes->PTS_DTS_flags = 0x02;                                        //10'：PTS字段存在,DTS不存在
	tsaacpes->ESCR_flag = 0x00;
	tsaacpes->ES_rate_flag = 0x00;
	tsaacpes->DSM_trick_mode_flag = 0x00;
	tsaacpes->additional_copy_info_flag = 0x00;
	tsaacpes->PES_CRC_flag = 0x00;
	tsaacpes->PES_extension_flag = 0x00;
	tsaacpes->PES_header_data_length = 0x05;                               //后面的数据 包括了PTS所占的字节数

	//清 0 
	tsaacpes->tsptsdts.pts_32_30 = 0;
	tsaacpes->tsptsdts.pts_29_15 = 0;
	tsaacpes->tsptsdts.pts_14_0 = 0;

	tsaacpes->tsptsdts.reserved_1 = 0x03;                                 //填写 pts信息
	// Adudiopts大于30bit，使用最高三位 
	if (Adudiopts > 0x7FFFFFFF) {
		tsaacpes->tsptsdts.pts_32_30 = (Adudiopts >> 30) & 0x07;
		tsaacpes->tsptsdts.marker_bit1 = 0x01;
	} else {
		tsaacpes->tsptsdts.marker_bit1 = 0;
	}

	// Videopts大于15bit，使用更多的位来存储
	if (Adudiopts > 0x7FFF) {
		tsaacpes->tsptsdts.pts_29_15 = (Adudiopts >> 15) & 0x007FFF;
		tsaacpes->tsptsdts.marker_bit2 = 0x01;
	} else {
		tsaacpes->tsptsdts.marker_bit2 = 0;
	}

	//使用最后15位
	tsaacpes->tsptsdts.pts_14_0 = Adudiopts & 0x007FFF;
	tsaacpes->tsptsdts.marker_bit3 = 0x01;

	return 0;
}

int H2642PES(TsPes *tsh264pes, unsigned long Videopts, unsigned char *pData, int iDataSize)
{
	memset(tsh264pes->Es, 0, MAX_ONE_FRAME_SIZE);

	memcpy(tsh264pes->Es, pData, iDataSize);

	tsh264pes->packet_start_code_prefix = 0x000001;
	tsh264pes->stream_id = TS_H264_STREAM_ID;                               //E0~EF表示是视频的,C0~DF是音频,H264-- E0
	tsh264pes->PES_packet_length = 0 ;                                      //一帧数据的长度 不包含 PES包头 ,这个8 是 自适应的长度,填0 可以自动查找
	tsh264pes->Pes_Packet_Length_Beyond = iDataSize;

	tsh264pes->marker_bit = 0x02;
	tsh264pes->PES_scrambling_control = 0x00;                               //人选字段 存在，不加扰
	tsh264pes->PES_priority = 0x00;
	tsh264pes->data_alignment_indicator = 0x00;
	tsh264pes->copyright = 0x00;
	tsh264pes->original_or_copy = 0x00;
	tsh264pes->PTS_DTS_flags = 0x02;                                         //10'：PTS字段存在,DTS不存在
	tsh264pes->ESCR_flag = 0x00;
	tsh264pes->ES_rate_flag = 0x00;
	tsh264pes->DSM_trick_mode_flag = 0x00;
	tsh264pes->additional_copy_info_flag = 0x00;
	tsh264pes->PES_CRC_flag = 0x00;
	tsh264pes->PES_extension_flag = 0x00;
	tsh264pes->PES_header_data_length = 0x05;                                //后面的数据 包括了PTS所占的字节数

	//清 0 
	tsh264pes->tsptsdts.pts_32_30  = 0;
	tsh264pes->tsptsdts.pts_29_15 = 0;
	tsh264pes->tsptsdts.pts_14_0 = 0;

	tsh264pes->tsptsdts.reserved_1 = 0x0003;                                 //填写 pts信息
	
	// Videopts大于30bit，使用最高三位 
	if(Videopts > 0x7FFFFFFF) {
		tsh264pes->tsptsdts.pts_32_30 = (Videopts >> 30) & 0x07;                 
		tsh264pes->tsptsdts.marker_bit1 = 0x01;
	} else {
		tsh264pes->tsptsdts.marker_bit1 = 0;
	}

	// Videopts大于15bit，使用更多的位来存储
	if(Videopts > 0x7FFF) {
		tsh264pes->tsptsdts.pts_29_15 = (Videopts >> 15) & 0x007FFF ;
		tsh264pes->tsptsdts.marker_bit2 = 0x01;
	} else {
		tsh264pes->tsptsdts.marker_bit2 = 0;
	}

	//使用最后15位
	tsh264pes->tsptsdts.pts_14_0 = Videopts & 0x007FFF;
	tsh264pes->tsptsdts.marker_bit3 = 0x01;

	return 0;
}

int WriteAAC2Ts(unsigned char *pData, int iDataSize)
{
	Ts_Adaptation_field  ts_adaptation_field_Head;
	Ts_Adaptation_field  ts_adaptation_field_Tail;

	AAC2PES(&m_audio_tspes, Timestamp_audio, pData, iDataSize);

	if (m_audio_tspes.Pes_Packet_Length_Beyond != 0) {
		printf("PES_AUDIO  :  SIZE = %d\n", m_audio_tspes.Pes_Packet_Length_Beyond);
		//填写自适应段标志
		WriteAdaptive_flags_Tail(&ts_adaptation_field_Head); //填写自适应段标志  ,这里注意 音频类型不要算pcr 所以都用帧尾代替就行
		WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾
		PES2TS(&m_audio_tspes, TS_AAC_PID, &ts_adaptation_field_Head, &ts_adaptation_field_Tail, Timestamp_video, Timestamp_audio);
		Timestamp_audio += 1024 * 1000 * 90 / 8000;
		//计算一帧音频所用时间
	}

	return 0;
}

int WriteH2642Ts(unsigned int framerate, unsigned char *pData, int iDataSize)
{
	unsigned int frametype = 0;    //视频帧类型
	Ts_Adaptation_field  ts_adaptation_field_Head;
	Ts_Adaptation_field  ts_adaptation_field_Tail;

	frametype = pData[4] & 0x1f;

	H2642PES(&m_video_tspes, Timestamp_video, pData, iDataSize);

	if (m_video_tspes.Pes_Packet_Length_Beyond != 0) {
	    printf("PES_VIDEO  :  SIZE = %d\n", m_video_tspes.Pes_Packet_Length_Beyond);
		if (frametype >= NAL_SLICE && frametype <= NAL_SLICE_IDR) {
			//填写自适应段标志
			WriteAdaptive_flags_Head(&ts_adaptation_field_Head, Timestamp_video); //填写自适应段标志帧头
			WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾
			//计算一帧视频所用时间
			PES2TS(&m_video_tspes, TS_H264_PID, &ts_adaptation_field_Head, &ts_adaptation_field_Tail, Timestamp_video, Timestamp_audio);
			Timestamp_video += 1000 * 90 / framerate;
		} else {
			//填写自适应段标志
			WriteAdaptive_flags_Tail(&ts_adaptation_field_Head); //填写自适应段标志  ,这里注意 其它帧类型不要算pcr 所以都用帧尾代替就行
			WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾
			PES2TS(&m_video_tspes, TS_H264_PID, &ts_adaptation_field_Head, &ts_adaptation_field_Tail, Timestamp_video, Timestamp_audio);
		}
	}
	
	return 0;
}


