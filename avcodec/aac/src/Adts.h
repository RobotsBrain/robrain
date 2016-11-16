#ifndef __ADTS_H__
#define __ADTS_H__

#include <stdint.h>
#include <stdio.h>



/*
 * ADTS 全称: Audio Data Transport Stream. 是 AAC音频的传输流格式.
 * ADTS头中相对有用的信息包括: 采样率, 声道数, 帧长度. 每一个带ADTS头信息的AAC流都会清晰的告诉解码器他需要的这些信息.
 * 一般情况下ADTS的头信息都是7个字节, 分为两部分: 前28bit是 AdtsFixedFeader, 后28bit是 AdtsVariableHeader.
 */

typedef struct {
    unsigned short syncword:				12;
    unsigned char id:                       1;
    unsigned char layer:					2;
    unsigned char protection_absent:        1;
    unsigned char profile:                  2;
    unsigned char sampling_frequency_index: 4;
    unsigned char private_bit:              1;
    unsigned char channel_configuration:	3;
    unsigned char original_copy:			1;
    unsigned char home:                     1;
} AdtsFixedFeader; // length : 28 bits

typedef struct {
    unsigned char copyright_identification_bit:		1;
    unsigned char copyright_identification_start:	1;
    unsigned short aac_frame_length:				13;
    unsigned short adts_buffer_fullness:			11;
    unsigned char number_of_raw_data_blocks_in_frame:2;
} AdtsVariableHeader; // length : 28 bits

// 设置adts固定的一部分
void SetFixedHeader(AdtsFixedFeader *header);

// 设置adts可变的一部分
void SetVariableHeader(AdtsVariableHeader *header, const int aac_raw_data_length);

// 解析buff中的adts固定部分头信息
void GetFixedHeader(const unsigned char buff[7], AdtsFixedFeader *header);

// 解析buff中的adts可变部分头信息
void GetVariableHeader(const unsigned char buff[7], AdtsVariableHeader *header);

// 将adts头信息转化为一个64位的整数, PS: 前面8位空着
void AdtsHeader2Int64(const AdtsFixedFeader *fixed_header, const AdtsVariableHeader *variable_header, uint64_t *header);

// 将adts头信息转化为一个7字节的buff
void AdtsHeader2Char(const AdtsFixedFeader *fixed_header, const AdtsVariableHeader *variable_header, unsigned char buffer[7]);

#endif