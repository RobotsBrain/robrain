#include "Audio.h"



int AAC2PES(TsPes *tsaacpes, unsigned long Adudiopts, unsigned char *pData, int iDataSize)
{
	unsigned int OneFrameLen_AAC = 0;

	//��ȡ��һ֡����
	OneFrameLen_AAC = iDataSize;
	memcpy(tsaacpes->Es, pData, iDataSize);

	tsaacpes->packet_start_code_prefix = 0x000001;
	tsaacpes->stream_id = TS_AAC_STREAM_ID;                                //E0~EF��ʾ����Ƶ��,C0~DF����Ƶ,H264-- E0
	tsaacpes->PES_packet_length = 0; // OneFrameLen_AAC + 8 ;             //һ֡���ݵĳ��� ������ PES��ͷ ,8����Ӧ�εĳ���
	tsaacpes->Pes_Packet_Length_Beyond = OneFrameLen_AAC;                  //= OneFrameLen_aac;     //���������һ֡  
	tsaacpes->marker_bit = 0x02;
	tsaacpes->PES_scrambling_control = 0x00;                               //��ѡ�ֶ� ���ڣ�������
	tsaacpes->PES_priority = 0x00;
	tsaacpes->data_alignment_indicator = 0x00;
	tsaacpes->copyright = 0x00;
	tsaacpes->original_or_copy = 0x00;
	tsaacpes->PTS_DTS_flags = 0x02;                                        //10'��PTS�ֶδ���,DTS������
	tsaacpes->ESCR_flag = 0x00;
	tsaacpes->ES_rate_flag = 0x00;
	tsaacpes->DSM_trick_mode_flag = 0x00;
	tsaacpes->additional_copy_info_flag = 0x00;
	tsaacpes->PES_CRC_flag = 0x00;
	tsaacpes->PES_extension_flag = 0x00;
	tsaacpes->PES_header_data_length = 0x05;                               //��������� ������PTS��ռ���ֽ���

	//�� 0 
	tsaacpes->tsptsdts.pts_32_30 = 0;
	tsaacpes->tsptsdts.pts_29_15 = 0;
	tsaacpes->tsptsdts.pts_14_0 = 0;

	tsaacpes->tsptsdts.reserved_1 = 0x03;                                 //��д pts��Ϣ
	// Adudiopts����30bit��ʹ�������λ 
	if (Adudiopts > 0x7FFFFFFF) {
		tsaacpes->tsptsdts.pts_32_30 = (Adudiopts >> 30) & 0x07;
		tsaacpes->tsptsdts.marker_bit1 = 0x01;
	} else {
		tsaacpes->tsptsdts.marker_bit1 = 0;
	}

	// Videopts����15bit��ʹ�ø����λ���洢
	if (Adudiopts > 0x7FFF) {
		tsaacpes->tsptsdts.pts_29_15 = (Adudiopts >> 15) & 0x007FFF;
		tsaacpes->tsptsdts.marker_bit2 = 0x01;
	} else {
		tsaacpes->tsptsdts.marker_bit2 = 0;
	}

	//ʹ�����15λ
	tsaacpes->tsptsdts.pts_14_0 = Adudiopts & 0x007FFF;
	tsaacpes->tsptsdts.marker_bit3 = 0x01;

	return 0;
}


