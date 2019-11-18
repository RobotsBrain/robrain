

#define TS_PACKET_HEADER               4
#define TS_PACKET_SIZE                 188
#define TS_SYNC_BYTE                   0x47
#define TS_PAT_PID                     0x00
#define TS_PMT_PID                     0xFFF
#define TS_H264_PID                    0x100
#define TS_AAC_PID                     0x101
#define TS_H264_STREAM_ID              0xE0
#define TS_AAC_STREAM_ID               0xC0
#define PMT_STREAM_TYPE_VIDEO          0x1B
#define PMT_STREAM_TYPE_AUDIO          0x0F
#define MAX_ONE_FRAME_SIZE             1024 * 1024

//ts °üÍ·
typedef struct {
	unsigned char sync_byte  	:8;  //Í¬²½×Ö½Ú, ¹Ì¶¨Îª0x47,±íÊ¾ºóÃæµÄÊÇÒ»¸öTS·Ö×é
	unsigned char tras_error 	:1;  //´«ÊäÎóÂëÖ¸Ê¾·û   
	unsigned char play_init     :1;   //ÓÐÐ§ºÉÔØµ¥ÔªÆðÊ¼Ö¸Ê¾·û
	unsigned char tras_prio	    :1;   //´«ÊäÓÅÏÈ, 1±íÊ¾¸ßÓÅÏÈ¼¶,´«Êä»úÖÆ¿ÉÄÜÓÃµ½£¬½âÂëÓÃ²»×Å
	unsigned int  PID           :13;  //PID
	unsigned char tras_scramb 	:2;  //´«Êä¼ÓÈÅ¿ØÖÆ
	unsigned char ada_field_C   :2; //×ÔÊÊÓ¦¿ØÖÆ 01½öº¬ÓÐÐ§¸ºÔØ£¬10½öº¬µ÷Õû×Ö¶Î£¬11º¬ÓÐµ÷Õû×Ö¶ÎºÍÓÐÐ§¸ºÔØ£¬ÏÈµ÷Õû×Ö¶ÎÈ»ºóÓÐÐ§¸ºÔØ¡£Îª00½âÂëÆ÷²»½øÐÐ´¦Àí 
	unsigned char conti_cter    :4;         //Á¬Ðø¼ÆÊýÆ÷ Ò»¸ö4bitµÄ¼ÆÊýÆ÷£¬·¶Î§0-15
} TsPacketHeader; 

//PAT½á¹¹Ìå£º½ÚÄ¿Ïà¹Ø±í
typedef struct {
	unsigned char table_id :8 ;                  //¹Ì¶¨Îª0x00 £¬±êÖ¾ÊÇ¸Ã±íÊÇPAT
	unsigned char section_syntax_indicator: 1;   //¶ÎÓï·¨±êÖ¾Î»£¬¹Ì¶¨Îª1
	unsigned char zero : 1;                      //0 
	unsigned char reserved_1 : 2;                //±£ÁôÎ»
	unsigned int section_length : 12 ;           //±íÊ¾Õâ¸ö×Ö½ÚºóÃæÓÐÓÃµÄ×Ö½ÚÊý£¬°üÀ¨CRC32
	unsigned int transport_stream_id : 16 ;      //¸Ã´«ÊäÁ÷µÄID£¬Çø±ðÓÚÒ»¸öÍøÂçÖÐÆäËü¶àÂ·¸´ÓÃµÄÁ÷
	unsigned char reserved_2 : 2;                //±£ÁôÎ»
	unsigned char version_number : 5 ;           //·¶Î§0-31£¬±íÊ¾PATµÄ°æ±¾ºÅ
	unsigned char current_next_indicator : 1 ;   //·¢ËÍµÄPATÊÇµ±Ç°ÓÐÐ§»¹ÊÇÏÂÒ»¸öPATÓÐÐ§
	unsigned char section_number : 8 ;           //·Ö¶ÎµÄºÅÂë¡£PAT¿ÉÄÜ·ÖÎª¶à¶Î´«Êä£¬µÚÒ»¶ÎÎª00£¬ÒÔºóÃ¿¸ö·Ö¶Î¼Ó1£¬×î¶à¿ÉÄÜÓÐ256¸ö·Ö¶Î
	unsigned char last_section_number : 8 ;      //×îºóÒ»¸ö·Ö¶ÎµÄºÅÂë
	unsigned int program_number  : 16 ;          //½ÚÄ¿ºÅ
	unsigned char reserved_3  : 3  ;             //±£ÁôÎ»
	unsigned int network_PID :13 ;               //ÍøÂçÐÅÏ¢±í£¨NIT£©µÄPID,½ÚÄ¿ºÅÎª0Ê±¶ÔÓ¦µÄPIDÎªnetwork_PID,±¾ÀýÖÐ²»º¬ÓÐ networke_pid
	unsigned int program_map_PID : 13;           //½ÚÄ¿Ó³Éä±íµÄPID£¬½ÚÄ¿ºÅ´óÓÚ0Ê±¶ÔÓ¦µÄPID£¬Ã¿¸ö½ÚÄ¿¶ÔÓ¦Ò»¸ö
	unsigned int CRC_32  : 32;             //CRC32Ð£ÑéÂë
} TsPat; 

//PMT½á¹¹Ìå£º½ÚÄ¿Ó³Éä±í
typedef struct {
	unsigned char table_id  : 8;                 //¹Ì¶¨Îª0x02, ±íÊ¾PMT±í
	unsigned char section_syntax_indicator : 1;  //¹Ì¶¨Îª0x01
	unsigned char zero: 1;                       //0x00
	unsigned char reserved_1 : 2;                //0x03
	unsigned int section_length: 12;             //Ê×ÏÈÁ½Î»bitÖÃÎª00£¬ËüÖ¸Ê¾¶ÎµÄbyteÊý£¬ÓÉ¶Î³¤¶ÈÓò¿ªÊ¼£¬°üº¬CRC¡£
	unsigned int program_number : 16;            // Ö¸³ö¸Ã½ÚÄ¿¶ÔÓ¦ÓÚ¿ÉÓ¦ÓÃµÄProgram map PID
	unsigned char reserved_2: 2;                 //0x03
	unsigned char version_number: 5;             //Ö¸³öTSÁ÷ÖÐProgram map sectionµÄ°æ±¾ºÅ
	unsigned char current_next_indicator: 1;     //µ±¸ÃÎ»ÖÃ1Ê±£¬µ±Ç°´«ËÍµÄProgram map section¿ÉÓÃ£»µ±¸ÃÎ»ÖÃ0Ê±£¬Ö¸Ê¾µ±Ç°´«ËÍµÄProgram map section²»¿ÉÓÃ£¬ÏÂÒ»¸öTSÁ÷µÄProgram map sectionÓÐÐ§¡£
	unsigned char section_number : 8;            //¹Ì¶¨Îª0x00
	unsigned char last_section_number: 8;        //¹Ì¶¨Îª0x00
	unsigned char reserved_3 : 3;                //0x07
	unsigned int PCR_PID : 13;                   //Ö¸Ã÷TS°üµÄPIDÖµ£¬¸ÃTS°üº¬ÓÐPCRÓò£¬¸ÃPCRÖµ¶ÔÓ¦ÓÚÓÉ½ÚÄ¿ºÅÖ¸¶¨µÄ¶ÔÓ¦½ÚÄ¿¡£Èç¹û¶ÔÓÚË½ÓÐÊý¾ÝÁ÷µÄ½ÚÄ¿¶¨ÒåÓëPCRÎÞ¹Ø£¬Õâ¸öÓòµÄÖµ½«Îª0x1FFF¡£
	unsigned char reserved_4 : 4;                //Ô¤ÁôÎª0x0F
	unsigned int program_info_length : 12;       //Ç°Á½Î»bitÎª00¡£¸ÃÓòÖ¸³ö¸úËæÆäºó¶Ô½ÚÄ¿ÐÅÏ¢µÄÃèÊöµÄbyteÊý¡£
	unsigned char stream_type_video : 8;         //Ö¸Ê¾ÌØ¶¨PIDµÄ½ÚÄ¿ÔªËØ°üµÄÀàÐÍ¡£¸Ã´¦PIDÓÉelementary PIDÖ¸¶¨
	unsigned char reserved_5_video: 3;           //0x07
	unsigned int elementary_PID_video: 13;       //¸ÃÓòÖ¸Ê¾TS°üµÄPIDÖµ¡£ÕâÐ©TS°üº¬ÓÐÏà¹ØµÄ½ÚÄ¿ÔªËØ
	unsigned char reserved_6_video : 4;          //0x0F
	unsigned int ES_info_length_video : 12;      //Ç°Á½Î»bitÎª00¡£¸ÃÓòÖ¸Ê¾¸úËæÆäºóµÄÃèÊöÏà¹Ø½ÚÄ¿ÔªËØµÄbyteÊý
	unsigned char stream_type_audio : 8;         //Ö¸Ê¾ÌØ¶¨PIDµÄ½ÚÄ¿ÔªËØ°üµÄÀàÐÍ¡£¸Ã´¦PIDÓÉelementary PIDÖ¸¶¨
	unsigned char reserved_5_audio: 3;           //0x07
	unsigned int elementary_PID_audio: 13;       //¸ÃÓòÖ¸Ê¾TS°üµÄPIDÖµ¡£ÕâÐ©TS°üº¬ÓÐÏà¹ØµÄ½ÚÄ¿ÔªËØ
	unsigned char reserved_6_audio : 4;          //0x0F
	unsigned int ES_info_length_audio : 12;      //Ç°Á½Î»bitÎª00¡£¸ÃÓòÖ¸Ê¾¸úËæÆäºóµÄÃèÊöÏà¹Ø½ÚÄ¿ÔªËØµÄbyteÊý
	unsigned long long CRC_32: 32;               //CRC32Ð£ÑéÂë
} TsPmt; 

//Á¬ÐøÐÔ¼ÆÊýÆ÷,Ò²¾ÍÊÇËµ ÓÐ¶àÉÙ¸ö pat°ü£¬¼¸¸öpmt°ü £¬¼¸¸öMP3 °ü£¬¼¸¸ö h264°ü£¬0x00 - 0x0f £¬È»ºóÕÛ»Øµ½0x00 ÓÃÓÚ²é¿´¶ª°ü                            
typedef struct {
	unsigned char continuity_counter_pat;
	unsigned char continuity_counter_pmt;
	unsigned char continuity_counter_video;
	unsigned char continuity_counter_audio;
} Continuity_Counter;

//×ÔÊÊÓ¦¶Î±êÖ¾
typedef struct {
	unsigned char discontinuty_indicator:1;                //1±íÃ÷µ±Ç°´«ËÍÁ÷·Ö×éµÄ²»Á¬Ðø×´Ì¬ÎªÕæ
	unsigned char random_access_indicator:1;               //±íÃ÷ÏÂÒ»¸öÓÐÏàÍ¬PIDµÄPES·Ö×éÓ¦¸Ãº¬ÓÐPTS×Ö¶ÎºÍÒ»¸öÔ­Ê¼Á÷·ÃÎÊµã
	unsigned char elementary_stream_priority_indicator:1;  //ÓÅÏÈ¼¶
	unsigned char PCR_flag:1;                              //°üº¬pcr×Ö¶Î
	unsigned char OPCR_flag:1;                             //°üº¬opcr×Ö¶Î
	unsigned char splicing_point_flag:1;                   //Æ´½Óµã±êÖ¾       
	unsigned char transport_private_data_flag:1;           //Ë½ÓÃ×Ö½Ú
	unsigned char adaptation_field_extension_flag:1;       //µ÷Õû×Ö¶ÎÓÐÀ©Õ¹

	unsigned char adaptation_field_length;                 //×ÔÊÊÓ¦¶Î³¤¶È
	unsigned long long  pcr;                               //×ÔÊÊÓ¦¶ÎÖÐÓÃµ½µÄµÄpcr
	unsigned long long  opcr;                              //×ÔÊÊÓ¦¶ÎÖÐÓÃµ½µÄµÄopcr
	unsigned char splice_countdown;
	unsigned char private_data_len;
	unsigned char private_data [256];
} Ts_Adaptation_field;

//PTS_DTS½á¹¹Ìå£º±¾³ÌÐòÉèÖÃ¶¼ÓÐ ¡°11¡±
typedef struct {
	unsigned char reserved_1 : 4;
	unsigned char pts_32_30  : 3;  //ÏÔÊ¾Ê±¼ä´Á
	unsigned char marker_bit1: 1;
	unsigned int  pts_29_15 : 15;
	unsigned char marker_bit2 : 1;
	unsigned int  pts_14_0 : 15;
	unsigned char marker_bit3 :1 ;
	unsigned char reserved_2 : 4;
	unsigned char dts_32_30: 3;   //½âÂëÊ±¼ä´Á
	unsigned char marker_bit4 :1;
	unsigned int  dts_29_15 :15;
	unsigned char marker_bit5: 1;
	unsigned int  dts_14_0 :15;
	unsigned char marker_bit6 :1 ;
} TsPtsDts;

//PES°ü½á¹¹Ìå£¬°üÀ¨PES°üÍ·ºÍESÊý¾Ý ,Í· 19 ¸ö×Ö½Ú
typedef struct {
	unsigned int   packet_start_code_prefix: 24;//ÆðÊ¼£º0x000001
	unsigned char  stream_id: 8;                //»ù±¾Á÷µÄÀàÐÍºÍ±àºÅ
	unsigned int   PES_packet_length: 16;       //°ü³¤¶È,¾ÍÊÇÖ¡Êý¾ÝµÄ³¤¶È£¬¿ÉÄÜÎª0,Òª×Ô¼ºËã,×ö¶à16Î»£¬Èç¹û³¬³öÔòÐèÒª×Ô¼ºËã
	unsigned char  marker_bit:2;                 //±ØÐëÊÇ£º'10'
	unsigned char  PES_scrambling_control:2;     //pes°üÓÐÐ§ÔØºÉµÄ¼ÓÈÅ·½Ê½
	unsigned char  PES_priority:1;               //ÓÐÐ§¸ºÔØµÄÓÅÏÈ¼¶
	unsigned char  data_alignment_indicator:1;   //Èç¹ûÉèÖÃÎª1±íÃ÷PES°üµÄÍ·ºóÃæ½ô¸ú×ÅÊÓÆµ»òÒôÆµsyncword¿ªÊ¼µÄ´úÂë¡£
	unsigned char  copyright:1;                  //1:¿¿°æÈ¨±£»¤£¬0£º²»¿¿
	unsigned char  original_or_copy:1;           //1;ÓÐÐ§¸ºÔØÊÇÔ­Ê¼µÄ£¬0£ºÓÐÐ§¸ºÔØÊ±¿½±´µÄ
	unsigned char  PTS_DTS_flags:2;              //'10'£ºPTS×Ö¶Î´æÔÚ£¬¡®11¡¯£ºPTDºÍDTS¶¼´æÔÚ£¬¡®00¡¯£º¶¼Ã»ÓÐ£¬¡®01¡¯£º½ûÓÃ¡£
	unsigned char  ESCR_flag:1;                  //1:escr»ù×¼×Ö¶Î ºÍ escrÀ©Õ¹×Ö¶Î¾ù´æÔÚ£¬0£ºÎÞÈÎºÎescr×Ö¶Î´æÔÚ
	unsigned char  ES_rate_flag:1;               //1:es_rate×Ö¶Î´æÔÚ£¬0 £º²»´æÔÚ
	unsigned char  DSM_trick_mode_flag:1;        //1;8±ÈÌØÌØ½Ó·½Ê½×Ö¶Î´æÔÚ£¬0 £º²»´æÔÚ
	unsigned char  additional_copy_info_flag:1;  //1:additional_copy_info´æÔÚ£¬0: ²»´æÔÚ
	unsigned char  PES_CRC_flag:1;               //1:crc×Ö¶Î´æÔÚ£¬0£º²»´æÔÚ
	unsigned char  PES_extension_flag:1;         //1£ºÀ©Õ¹×Ö¶Î´æÔÚ£¬0£º²»´æÔÚ
	unsigned char  PES_header_data_length :8;    //ºóÃæÊý¾ÝµÄ³¤¶È£¬
	TsPtsDts       tsptsdts;                     //ptsdts½á¹¹Ìå¶ÔÏó£¬10¸ö×Ö½Ú
	unsigned char  Es[MAX_ONE_FRAME_SIZE];       //Ò»Ö¡ Ô­Ê¼Êý¾Ý
	unsigned int   Pes_Packet_Length_Beyond;     //Èç¹ûPES_packet_lengthµÄ´óÐ¡²»ÄÜÂú×ãÒ»Ö¡Êý¾ÝµÄ³¤¶ÈÔòÓÃÕâ¸ö´úÌæ
} TsPes;

typedef void (*STREAMCALLBACK)(unsigned char *data, int len);

class TsMux
{
public:
	TsMux();
	~TsMux();

	void SetCallback(STREAMCALLBACK pcbk);

	int WriteAAC2Ts(unsigned char *pData, int iDataSize);

	int WriteH2642Ts(unsigned int framerate, unsigned char *pData, int iDataSize);
	
private:
	int WriteAdaptive_flags_Head(Ts_Adaptation_field  *ts_adaptation_field, unsigned int Videopts);
	int WriteAdaptive_flags_Tail(Ts_Adaptation_field  *ts_adaptation_field);
	int CreateAdaptive_Ts(Ts_Adaptation_field *ts_adaptation_field, unsigned char *buf, unsigned int AdaptiveLength);
	int PES2TS(TsPes *ts_pes, unsigned int avPid, Ts_Adaptation_field *ts_adaptation_field_Head, Ts_Adaptation_field *ts_adaptation_field_Tail,
		   unsigned long  Videopts, unsigned long Adudiopts);
	int AAC2PES(TsPes *tsaacpes, unsigned long Adudiopts, unsigned char *pData, int iDataSize);
	int H2642PES(TsPes *tsh264pes, unsigned long Videopts, unsigned char *pData, int iDataSize);

	int WriteStruct_Packetheader(unsigned char *Buf, unsigned int PID, unsigned char play_init, unsigned char ada_field_C);
	int WriteStruct_Pat(unsigned char *Buf);
	int WriteStruct_Pmt(unsigned char *Buf);

private:
	TsPes m_video_tspes;
	TsPes m_audio_tspes;
	unsigned int m_WritePacketNum;
	unsigned long  m_Timestamp_video;
	unsigned long  m_Timestamp_audio;
	STREAMCALLBACK m_cbk;

	Continuity_Counter m_continuity_counter;     //°üÀàÐÍ¼ÆÊýÆ÷
};






