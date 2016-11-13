#include "Audio.h"



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


