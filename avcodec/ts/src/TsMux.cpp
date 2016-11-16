#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "TsMux.h"


enum nal_unit_type_e {
	NAL_UNKNOWN     = 0,
	NAL_SLICE       = 1,
	NAL_SLICE_DPA   = 2,
	NAL_SLICE_DPB   = 3,
	NAL_SLICE_DPC   = 4,
	NAL_SLICE_IDR   = 5,    /* ref_idc != 0 */
	NAL_SEI         = 6,    /* ref_idc == 0 */
	NAL_SPS         = 7,
	NAL_PPS         = 8
	/* ref_idc == 0 for 6,9,10,11,12 */
};

static unsigned int crc_tab[256] = {
  	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
  	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
  	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
  	0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
  	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
  	0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
  	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
  	0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
  	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
  	0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
  	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
  	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
  	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
  	0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
  	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
  	0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
  	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
  	0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
  	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
  	0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
  	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
  	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
  	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
  	0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
  	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
  	0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
  	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
  	0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
  	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
  	0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
  	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
  	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
  	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
  	0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
  	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
  	0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
  	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
  	0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
  	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
  	0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
  	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
  	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
  	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static unsigned int calc_crc32(unsigned char *data, unsigned int datalen)
{
  	unsigned int i;
  	unsigned int crc = 0xffffffff;

  	for (i=0; i<datalen; i++) {
    	crc = (crc << 8) ^ crc_tab[((crc >> 24) ^ *data++) & 0xff];
  	}

  	return crc;
}

TsMux::TsMux()
{

}

TsMux::~TsMux()
{

}

int TsMux::WriteStruct_Packetheader(unsigned char *Buf, unsigned int PID, unsigned char play_init, unsigned char ada_field_C)
{
	TsPacketHeader tspacketheader;

	tspacketheader.sync_byte = TS_SYNC_BYTE;
	tspacketheader.tras_error = 0x00;
	tspacketheader.play_init = play_init;
	tspacketheader.tras_prio = 0x00;
	tspacketheader.PID = PID;
	tspacketheader.tras_scramb = 0x00;
	tspacketheader.ada_field_C = ada_field_C;

	if (PID == TS_PAT_PID) {
		tspacketheader.conti_cter = (m_continuity_counter.continuity_counter_pat %16);
		m_continuity_counter.continuity_counter_pat++;
	} else if (PID == TS_PMT_PID) {
		tspacketheader.conti_cter = (m_continuity_counter.continuity_counter_pmt %16);
		m_continuity_counter.continuity_counter_pmt++;
	} else if (PID == TS_H264_PID ) {
		tspacketheader.conti_cter = (m_continuity_counter.continuity_counter_video %16);
		m_continuity_counter.continuity_counter_video ++;
	} else if (PID == TS_AAC_PID) {
		tspacketheader.conti_cter = (m_continuity_counter.continuity_counter_audio %16);
		m_continuity_counter.continuity_counter_audio ++;
	} else {
		printf("continuity_counter error packet\n");
		getchar();
	}

	Buf[0] = tspacketheader.sync_byte;
	Buf[1] = tspacketheader.tras_error << 7 | tspacketheader.play_init << 6  | tspacketheader.tras_prio << 5 | ((tspacketheader.PID >> 8) & 0x1f);
	Buf[2] = (tspacketheader. PID & 0x00ff);
	Buf[3] = tspacketheader.tras_scramb << 6 | tspacketheader.ada_field_C << 4 | tspacketheader.conti_cter;
	
	return 4;
}

int TsMux::WriteStruct_Pat(unsigned char *Buf)
{
	unsigned int pat_pos = 0;  
	TsPat tspat;
	unsigned int PAT_CRC = 0xFFFFFFFF;

	memset(Buf, 0xFF, TS_PACKET_SIZE);

	tspat.table_id = 0x00;
	tspat.section_syntax_indicator = 0x01;
	tspat.zero = 0x00;
	tspat.reserved_1 = 0x03;  //设置为11；
	tspat.section_length = 0x0d;    //pat结构体长度 16个字节减去上面的3个字节
	tspat.transport_stream_id = 0x01;
	tspat.reserved_2 = 0x03;     //设置为11；
	tspat.version_number = 0x00;
	tspat.current_next_indicator = 0x01;   //当前的pat 有效
	tspat.section_number = 0x00;
	tspat.last_section_number = 0x00;
	tspat.program_number = 0x01;
	tspat.reserved_3 = 0x07;     //设置为111；
	tspat.program_map_PID = TS_PMT_PID;   //PMT的PID
	tspat.CRC_32 = PAT_CRC;   //传输过程中检测的一种算法值 先设定一个填充值

	//PID = 0x00,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x01,仅有有效负载
	pat_pos += WriteStruct_Packetheader(Buf,TS_PAT_PID,0x01,0x01);  
	Buf[4] = 0;   //自适应段的长度为0
	pat_pos ++;

	Buf[pat_pos] = tspat.table_id;
	Buf[pat_pos + 1] = tspat.section_syntax_indicator << 7 | tspat.zero  << 6 | tspat.reserved_1 << 4 | ((tspat.section_length >> 8) & 0x0F);
	Buf[pat_pos + 2] = tspat.section_length & 0x00FF;
	Buf[pat_pos + 3] = tspat.transport_stream_id >> 8;
	Buf[pat_pos + 4] = tspat.transport_stream_id & 0x00FF;
	Buf[pat_pos + 5] = tspat.reserved_2 << 6 | tspat.version_number << 1 | tspat.current_next_indicator;
	Buf[pat_pos + 6] = tspat.section_number;
	Buf[pat_pos + 7] = tspat.last_section_number;
	Buf[pat_pos + 8] = tspat.program_number>>8;
	Buf[pat_pos + 9] = tspat.program_number & 0x00FF;
	Buf[pat_pos + 10]= tspat.reserved_3 << 5 | ((tspat.program_map_PID >> 8) & 0x0F);
	Buf[pat_pos + 11]= tspat.program_map_PID & 0x00FF;
	pat_pos += 12;

	PAT_CRC = ntohl(calc_crc32(Buf + 5, pat_pos - 5));
	memcpy(Buf + pat_pos, (unsigned char *)&PAT_CRC, 4);
	pat_pos += 4; 

	return 188;
}

int TsMux::WriteStruct_Pmt(unsigned char * Buf)
{
	unsigned int pmt_pos = 0;  
	TsPmt tspmt;
	unsigned long long PMT_CRC = 0xFFFFFFFF; 
	int len = 0;

	memset(Buf,0xFF,TS_PACKET_SIZE);

	tspmt.table_id = 0x02;
	tspmt.section_syntax_indicator = 0x01;
	tspmt.zero = 0x00;
	tspmt.reserved_1 = 0x03;
	tspmt.section_length = 0x17;    //PMT结构体长度 16 + 5 + 5个字节减去上面的3个字节
	tspmt.program_number = 01;   //只有一个节目
	tspmt.reserved_2 = 0x03;
	tspmt.version_number = 0x00;
	tspmt.current_next_indicator = 0x01;  //当前的PMT有效
	tspmt.section_number = 0x00;
	tspmt.last_section_number = 0x00;
	tspmt.reserved_3 = 0x07;
	tspmt.PCR_PID = TS_H264_PID;    //视频PID                                   
	tspmt.reserved_4 = 0x0F;
	tspmt.program_info_length = 0x00;   //后面无 节目信息描述
	tspmt.stream_type_video = PMT_STREAM_TYPE_VIDEO;   //视频的类型
	tspmt.reserved_5_video = 0x07;
	tspmt.elementary_PID_video = TS_H264_PID;   //视频的PID
	tspmt.reserved_6_video= 0x0F;
	tspmt.ES_info_length_video = 0x00;  //视频无跟随的相关信息
	tspmt.stream_type_audio = PMT_STREAM_TYPE_AUDIO;   //音频类型
	tspmt.reserved_5_audio = 0x07;
	tspmt.elementary_PID_audio = TS_AAC_PID;  //音频PID 
	tspmt.reserved_6_audio = 0x0F;
	tspmt.ES_info_length_audio = 0x00;    //音频无跟随的相关信息
	tspmt.CRC_32 = PMT_CRC; 

	//PID = TS_PMT_PID,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x01,仅有有效负载；
	pmt_pos += WriteStruct_Packetheader(Buf, TS_PMT_PID, 0x01, 0x01);           
	Buf[4] = 0;    //自适应段的长度为0
	pmt_pos ++;

	Buf[pmt_pos + 0] = tspmt.table_id;
	Buf[pmt_pos + 1] = tspmt.section_syntax_indicator << 7 | tspmt.zero  << 6 | tspmt.reserved_1 << 4 | ((tspmt.section_length >> 8) & 0x0F);
	Buf[pmt_pos + 2] = tspmt.section_length & 0x00FF;
	Buf[pmt_pos + 3] = tspmt.program_number >> 8;
	Buf[pmt_pos + 4] = tspmt.program_number & 0x00FF;
	Buf[pmt_pos + 5] = tspmt.reserved_2 << 6 | tspmt.version_number << 1 | tspmt.current_next_indicator;
	Buf[pmt_pos + 6] = tspmt.section_number;
	Buf[pmt_pos + 7] = tspmt.last_section_number;
	Buf[pmt_pos + 8] = tspmt.reserved_3 << 5  | ((tspmt.PCR_PID >> 8) & 0x1F);
	Buf[pmt_pos + 9] = tspmt.PCR_PID & 0x0FF;
	Buf[pmt_pos + 10]= tspmt.reserved_4 << 4 | ((tspmt.program_info_length >> 8) & 0x0F);
	Buf[pmt_pos + 11]= tspmt.program_info_length & 0xFF;
	Buf[pmt_pos + 12]= tspmt.stream_type_video;   //视频流的stream_type
	Buf[pmt_pos + 13]= tspmt.reserved_5_video << 5 | ((tspmt.elementary_PID_video >> 8 ) & 0x1F);
	Buf[pmt_pos + 14]= tspmt.elementary_PID_video & 0x00FF;
	Buf[pmt_pos + 15]= tspmt.reserved_6_video<< 4 | ((tspmt.ES_info_length_video >> 8) & 0x0F);
	Buf[pmt_pos + 16]= tspmt.ES_info_length_video & 0x0FF;
	Buf[pmt_pos + 17]= tspmt.stream_type_audio;   //音频流的stream_type
	Buf[pmt_pos + 18]= tspmt.reserved_5_audio<< 5 | ((tspmt.elementary_PID_audio >> 8 ) & 0x1F);
	Buf[pmt_pos + 19]= tspmt.elementary_PID_audio & 0x00FF;
	Buf[pmt_pos + 20]= tspmt.reserved_6_audio << 4 | ((tspmt.ES_info_length_audio >> 8) & 0x0F);
	Buf[pmt_pos + 21]= tspmt.ES_info_length_audio & 0x0FF;
	pmt_pos += 22;

	len  = pmt_pos - 8 + 4;
	len = len > 0xffff ? 0:len;
	Buf[6] = 0xb0 | (len >> 8);
	Buf[7] = len;

	PMT_CRC = ntohl(calc_crc32(Buf + 5, pmt_pos - 5));
	memcpy(Buf + pmt_pos , (unsigned char *)&PMT_CRC, 4);
	pmt_pos += 4;

	return 188;
}

void TsMux::SetCallback(STREAMCALLBACK pcbk)
{
	m_cbk = pcbk;
}

int TsMux::WriteAdaptive_flags_Head(Ts_Adaptation_field  *ts_adaptation_field, unsigned int Videopts)
{
	//填写自适应段
	ts_adaptation_field->discontinuty_indicator = 0;
	ts_adaptation_field->random_access_indicator = 0;
	ts_adaptation_field->elementary_stream_priority_indicator = 0;
	ts_adaptation_field->PCR_flag = 1;	//只用到这个
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

int TsMux::WriteAdaptive_flags_Tail(Ts_Adaptation_field  *ts_adaptation_field)
{
	//填写自适应段
	ts_adaptation_field->discontinuty_indicator = 0;
	ts_adaptation_field->random_access_indicator = 0;
	ts_adaptation_field->elementary_stream_priority_indicator = 0;
	ts_adaptation_field->PCR_flag = 0;	//只用到这个
	ts_adaptation_field->OPCR_flag = 0;
	ts_adaptation_field->splicing_point_flag = 0;
	ts_adaptation_field->transport_private_data_flag = 0;
	ts_adaptation_field->adaptation_field_extension_flag = 0;

	//需要自己算
	ts_adaptation_field->pcr  = 0;
	ts_adaptation_field->adaptation_field_length = 1;	//占用1位标志所用的位

	ts_adaptation_field->opcr = 0;
	ts_adaptation_field->splice_countdown = 0;
	ts_adaptation_field->private_data_len = 0;

	return 1;
}

int TsMux::CreateAdaptive_Ts(Ts_Adaptation_field *ts_adaptation_field, unsigned char *buf, unsigned int AdaptiveLength)
{
	unsigned int CurrentAdaptiveLength = 1; 	//当前已经用的自适应段长度  
	unsigned char Adaptiveflags = 0;	//自适应段的标志
	unsigned int adaptive_pos = 0;

	//填写自适应字段
	if (ts_adaptation_field->adaptation_field_length > 0) {
		adaptive_pos += 1;	//自适应段的一些标志所占用的1个字节
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

		buf[0] = Adaptiveflags;		//将标志放入内存
	}

	return 1;
}

int TsMux::PES2TS(TsPes *ts_pes, unsigned int avPid, Ts_Adaptation_field *ts_adaptation_field_Head, Ts_Adaptation_field *ts_adaptation_field_Tail,
		   unsigned long  Videopts, unsigned long Adudiopts)
{
	TsPacketHeader ts_header;
	unsigned int ts_pos = 0;
	unsigned int FirstPacketLoadLength = 0;  	 //分片包的第一个包的负载长度
	unsigned int NeafPacketCount = 0; 	//分片包的个数
	unsigned int AdaptiveLength = 0; 	//要填写0XFF的长度
	unsigned char *NeafBuf = NULL;  	//分片包 总负载的指针
	unsigned char TSbuf[TS_PACKET_SIZE];

	memset(TSbuf, 0, TS_PACKET_SIZE); 
	FirstPacketLoadLength = 188 - 4 - 1 - ts_adaptation_field_Head->adaptation_field_length - 14; //计算分片包的第一个包的负载长度
	NeafPacketCount += 1; 	//第一个分片包  

	//一个包的情况
	if (ts_pes->Pes_Packet_Length_Beyond < FirstPacketLoadLength) { //这里是 sps ，pps ，sei等
		memset(TSbuf, 0xFF, TS_PACKET_SIZE);

		//PID = TS_H264_PID,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x03,含有调整字段和有效负载
		WriteStruct_Packetheader(TSbuf, avPid, 0x01, 0x03);
		
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

		m_WritePacketNum++; //已经写入文件的包个数++
		
		return m_WritePacketNum;
	}
	
	NeafPacketCount += (ts_pes->Pes_Packet_Length_Beyond - FirstPacketLoadLength)/ 184;     
	NeafPacketCount += 1;   //最后一个分片包
	AdaptiveLength = 188 - 4 - 1 - ((ts_pes->Pes_Packet_Length_Beyond - FirstPacketLoadLength)% 184)  ;  //要填写0XFF的长度
	
	if ((m_WritePacketNum % 40) == 0) {   //每40个包打一个 pat,一个pmt
		u_char buf[TS_PACKET_SIZE] = {0};

		WriteStruct_Pat(buf);
		m_cbk(buf, TS_PACKET_SIZE);

		WriteStruct_Pmt(buf);
		m_cbk(buf, TS_PACKET_SIZE);
	}

	//开始处理第一个包,分片包的个数最少也会是两个 
	//PID = TS_H264_PID,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x03,含有调整字段和有效负载
	WriteStruct_Packetheader(TSbuf, avPid, 0x01, 0x03);                              
	ts_pos += 4;
	TSbuf[ts_pos] = ts_adaptation_field_Head->adaptation_field_length;  //自适应字段的长度，自己填写的
	ts_pos += 1;                                                       

	CreateAdaptive_Ts(ts_adaptation_field_Head, TSbuf + ts_pos, (188 - 4 - 1 - 14));  //填写自适应字段
	ts_pos += ts_adaptation_field_Head->adaptation_field_length;   //填写自适应段所需要的长度

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

	m_WritePacketNum++;  //已经写入文件的包个数++

	while(ts_pes->Pes_Packet_Length_Beyond) {
		ts_pos = 0;
		memset(TSbuf, 0, TS_PACKET_SIZE); 

		if ((m_WritePacketNum % 40) == 0) {  //每40个包打一个 pat,一个pmt
			u_char buf[TS_PACKET_SIZE] = {0};

			WriteStruct_Pat(buf);
			m_cbk(buf, TS_PACKET_SIZE);

			WriteStruct_Pmt(buf);
			m_cbk(buf, TS_PACKET_SIZE);
		}

		if(ts_pes->Pes_Packet_Length_Beyond >= 184) {
			//处理中间包   
			//PID = TS_H264_PID,不是有效荷载单元起始指示符_play_init = 0x00, ada_field_C,0x01,仅有有效负载
			WriteStruct_Packetheader(TSbuf, avPid, 0x00, 0x01);
			ts_pos += 4;
            memcpy(TSbuf + ts_pos,NeafBuf,184); 
			NeafBuf += 184;
			ts_pes->Pes_Packet_Length_Beyond -= 184;
			m_cbk(TSbuf, TS_PACKET_SIZE);
		} else {
			if(ts_pes->Pes_Packet_Length_Beyond == 183 || ts_pes->Pes_Packet_Length_Beyond == 182) {
				//PID = TS_H264_PID,不是有效荷载单元起始指示符_play_init = 0x00, ada_field_C,0x03,含有调整字段和有效负载
				WriteStruct_Packetheader(TSbuf, avPid, 0x00, 0x03);   
				ts_pos += 4;
				TSbuf[ts_pos + 0] = 0x01;
				TSbuf[ts_pos + 1] = 0x00;
				ts_pos += 2;
				memcpy(TSbuf + ts_pos,NeafBuf,182); 
				  
				NeafBuf += 182;
				ts_pes->Pes_Packet_Length_Beyond -= 182;
			} else {
				//PID = TS_H264_PID,不是有效荷载单元起始指示符_play_init = 0x00, ada_field_C,0x03,含有调整字段和有效负载
				WriteStruct_Packetheader(TSbuf, avPid, 0x00, 0x03);  
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

		m_WritePacketNum++;  
	}

	return m_WritePacketNum ;
}

int TsMux::AAC2PES(TsPes *tsaacpes, unsigned long Adudiopts, unsigned char *pData, int iDataSize)
{
	unsigned int OneFrameLen_AAC = 0;

	//读取出一帧数据
	OneFrameLen_AAC = iDataSize;
	memcpy(tsaacpes->Es, pData, iDataSize);

	tsaacpes->packet_start_code_prefix = 0x000001;
	tsaacpes->stream_id = TS_AAC_STREAM_ID;     //E0~EF表示是视频的,C0~DF是音频,H264-- E0
	tsaacpes->PES_packet_length = 0; // OneFrameLen_AAC + 8 ;   //一帧数据的长度 不包含 PES包头 ,8自适应段的长度
	tsaacpes->Pes_Packet_Length_Beyond = OneFrameLen_AAC; 
	tsaacpes->marker_bit = 0x02;
	tsaacpes->PES_scrambling_control = 0x00;    //人选字段 存在，不加扰
	tsaacpes->PES_priority = 0x00;
	tsaacpes->data_alignment_indicator = 0x00;
	tsaacpes->copyright = 0x00;
	tsaacpes->original_or_copy = 0x00;
	tsaacpes->PTS_DTS_flags = 0x02;    //10'：PTS字段存在,DTS不存在
	tsaacpes->ESCR_flag = 0x00;
	tsaacpes->ES_rate_flag = 0x00;
	tsaacpes->DSM_trick_mode_flag = 0x00;
	tsaacpes->additional_copy_info_flag = 0x00;
	tsaacpes->PES_CRC_flag = 0x00;
	tsaacpes->PES_extension_flag = 0x00;
	tsaacpes->PES_header_data_length = 0x05;   //后面的数据 包括了PTS所占的字节数

	//清 0 
	tsaacpes->tsptsdts.pts_32_30 = 0;
	tsaacpes->tsptsdts.pts_29_15 = 0;
	tsaacpes->tsptsdts.pts_14_0 = 0;

	tsaacpes->tsptsdts.reserved_1 = 0x03;   //填写 pts信息
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

int TsMux::H2642PES(TsPes *tsh264pes, unsigned long Videopts, unsigned char *pData, int iDataSize)
{
	memset(tsh264pes->Es, 0, MAX_ONE_FRAME_SIZE);

	memcpy(tsh264pes->Es, pData, iDataSize);

	tsh264pes->packet_start_code_prefix = 0x000001;
	tsh264pes->stream_id = TS_H264_STREAM_ID;  //E0~EF表示是视频的,C0~DF是音频,H264-- E0
	tsh264pes->PES_packet_length = 0 ;   //一帧数据的长度 不包含 PES包头 ,这个8 是 自适应的长度,填0 可以自动查找
	tsh264pes->Pes_Packet_Length_Beyond = iDataSize;

	tsh264pes->marker_bit = 0x02;
	tsh264pes->PES_scrambling_control = 0x00;   //存在，不加扰
	tsh264pes->PES_priority = 0x00;
	tsh264pes->data_alignment_indicator = 0x00;
	tsh264pes->copyright = 0x00;
	tsh264pes->original_or_copy = 0x00;
	tsh264pes->PTS_DTS_flags = 0x02;   //10'：PTS字段存在,DTS不存在
	tsh264pes->ESCR_flag = 0x00;
	tsh264pes->ES_rate_flag = 0x00;
	tsh264pes->DSM_trick_mode_flag = 0x00;
	tsh264pes->additional_copy_info_flag = 0x00;
	tsh264pes->PES_CRC_flag = 0x00;
	tsh264pes->PES_extension_flag = 0x00;
	tsh264pes->PES_header_data_length = 0x05;  //后面的数据 包括了PTS所占的字节数

	//清 0 
	tsh264pes->tsptsdts.pts_32_30  = 0;
	tsh264pes->tsptsdts.pts_29_15 = 0;
	tsh264pes->tsptsdts.pts_14_0 = 0;

	tsh264pes->tsptsdts.reserved_1 = 0x0003;  //填写 pts信息
	
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

int TsMux::WriteAAC2Ts(unsigned char *pData, int iDataSize)
{
	Ts_Adaptation_field  ts_adaptation_field_Head;
	Ts_Adaptation_field  ts_adaptation_field_Tail;

	AAC2PES(&m_audio_tspes, m_Timestamp_audio, pData, iDataSize);

	if (m_audio_tspes.Pes_Packet_Length_Beyond != 0) {
		printf("PES_AUDIO  :  SIZE = %d\n", m_audio_tspes.Pes_Packet_Length_Beyond);

		WriteAdaptive_flags_Tail(&ts_adaptation_field_Head); //填写自适应段标志  ,这里注意 音频类型不要算pcr 所以都用帧尾代替就行
		WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾
		PES2TS(&m_audio_tspes, TS_AAC_PID, &ts_adaptation_field_Head, &ts_adaptation_field_Tail, m_Timestamp_video, m_Timestamp_audio);
		m_Timestamp_audio += 1024 * 1000 * 90 / 8000;
		//计算一帧音频所用时间
	}

	return 0;
}

int TsMux::WriteH2642Ts(unsigned int framerate, unsigned char *pData, int iDataSize)
{
	unsigned int frametype = 0;
	Ts_Adaptation_field  ts_adaptation_field_Head;
	Ts_Adaptation_field  ts_adaptation_field_Tail;

	frametype = pData[4] & 0x1f;

	H2642PES(&m_video_tspes, m_Timestamp_video, pData, iDataSize);

	if (m_video_tspes.Pes_Packet_Length_Beyond != 0) {
	    printf("PES_VIDEO  :  SIZE = %d\n", m_video_tspes.Pes_Packet_Length_Beyond);
		if (frametype >= NAL_SLICE && frametype <= NAL_SLICE_IDR) {
			WriteAdaptive_flags_Head(&ts_adaptation_field_Head, m_Timestamp_video); //填写自适应段标志帧头
			WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾
			//计算一帧视频所用时间
			PES2TS(&m_video_tspes, TS_H264_PID, &ts_adaptation_field_Head, &ts_adaptation_field_Tail, m_Timestamp_video, m_Timestamp_audio);
			m_Timestamp_video += 1000 * 90 / framerate;
		} else {
			WriteAdaptive_flags_Tail(&ts_adaptation_field_Head); //填写自适应段标志  ,这里注意 其它帧类型不要算pcr 所以都用帧尾代替就行
			WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾
			PES2TS(&m_video_tspes, TS_H264_PID, &ts_adaptation_field_Head, &ts_adaptation_field_Tail, m_Timestamp_video, m_Timestamp_audio);
		}
	}
	
	return 0;
}


