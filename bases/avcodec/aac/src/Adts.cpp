#include <math.h>

#include "Adts.h"



void SetFixedHeader(AdtsFixedFeader *header) 
{
    header->syncword                 = 0xFFF;// 代表着一个帧的开始
    header->id                       = 0;// MPEG Version: 0 for MPEG-4, 1 for MPEG-2
    header->layer                    = 0;// always 00
    header->protection_absent        = 1;// 表示是否误码校验
    header->profile                  = 1;// 表示使用哪个级别的AAC, 有些芯片只支持AAC LC.
    header->sampling_frequency_index = 4;// 表示使用的采样率的下标
    header->private_bit              = 0;
    header->channel_configuration    = 2;// 表示声道数
    header->original_copy            = 0;
    header->home                     = 0;

    return;
}

void SetVariableHeader(AdtsVariableHeader *header, const int size) 
{
    header->copyright_identification_bit       = 0;
    header->copyright_identification_start     = 0;
    header->aac_frame_length                   = size + 7;
    header->adts_buffer_fullness               = 0x7f;
    header->number_of_raw_data_blocks_in_frame = 2;

    return;
}

void GetFixedHeader(const unsigned char buff[7], AdtsFixedFeader *header) 
{
    unsigned long long adts = 0;
    const unsigned char *p = buff;

    adts |= *p ++; adts <<= 8;
    adts |= *p ++; adts <<= 8;
    adts |= *p ++; adts <<= 8;
    adts |= *p ++; adts <<= 8;
    adts |= *p ++; adts <<= 8;
    adts |= *p ++; adts <<= 8;
    adts |= *p ++;
    
    header->syncword                 = (adts >> 44);
    header->id                       = (adts >> 43) & 0x01;
    header->layer                    = (adts >> 41) & 0x03;
    header->protection_absent        = (adts >> 40) & 0x01;
    header->profile                  = (adts >> 38) & 0x03;
    header->sampling_frequency_index = (adts >> 34) & 0x0e;
    header->private_bit              = (adts >> 33) & 0x01;
    header->channel_configuration    = (adts >> 30) & 0x07;
    header->original_copy            = (adts >> 29) & 0x01;
    header->home                     = (adts >> 28) & 0x01;

    return;
}

void GetVariableHeader(const unsigned char buff[7], AdtsVariableHeader *header) 
{
    unsigned long long adts = 0;

    adts  = buff[0]; adts <<= 8;
    adts |= buff[1]; adts <<= 8;
    adts |= buff[2]; adts <<= 8;
    adts |= buff[3]; adts <<= 8;
    adts |= buff[4]; adts <<= 8;
    adts |= buff[5]; adts <<= 8;
    adts |= buff[6];
    
    header->copyright_identification_bit = (adts >> 27) & 0x01;
    header->copyright_identification_start = (adts >> 26) & 0x01;
    header->aac_frame_length = (adts >> 13) & ((int)pow(2, 14) - 1);
    header->adts_buffer_fullness = (adts >> 2) & ((int)pow(2, 11) - 1);
    header->number_of_raw_data_blocks_in_frame = adts & 0x03;

    return;
}

void AdtsHeader2Int64(const AdtsFixedFeader *fixed_header, const AdtsVariableHeader *variable_header, uint64_t *header) 
{
    uint64_t ret_value = 0;

    ret_value = fixed_header->syncword;
    ret_value <<= 1;
    ret_value |= fixed_header->id;
    ret_value <<= 2;
    ret_value |= fixed_header->layer;
    ret_value <<= 1;
    ret_value |= (fixed_header->protection_absent) & 0x01;
    ret_value <<= 2;
    ret_value |= fixed_header->profile;
    ret_value <<= 4;
    ret_value |= (fixed_header->sampling_frequency_index) & 0x0e;
    ret_value <<= 1;
    ret_value |= (fixed_header->private_bit) & 0x01;
    ret_value <<= 3;
    ret_value |= (fixed_header->channel_configuration) & 0x07;
    ret_value <<= 1;
    ret_value |= (fixed_header->original_copy) & 0x01;
    ret_value <<= 1;
    ret_value |= (fixed_header->home) & 0x01;
    
    ret_value <<= 1;
    ret_value |= (variable_header->copyright_identification_bit) & 0x01;
    ret_value <<= 1;
    ret_value |= (variable_header->copyright_identification_start) & 0x01;
    ret_value <<= 13;
    ret_value |= (variable_header->aac_frame_length) & ((int)pow(2, 13) - 1);
    ret_value <<= 11;
    ret_value |= (variable_header->adts_buffer_fullness) & ((int)pow(2, 11) - 1);
    ret_value <<= 2;
    ret_value |= (variable_header->number_of_raw_data_blocks_in_frame) & ((int)pow(2, 2) - 1);
    
    *header = ret_value;

    return;
}

void AdtsHeader2Char(const AdtsFixedFeader *fixed_header, const AdtsVariableHeader *variable_header, unsigned char buffer[7]) 
{
    uint64_t value = 0;

    AdtsHeader2Int64(fixed_header, variable_header, &value);
    
    buffer[0] = (value >> 48) & 0xff;
    buffer[1] = (value >> 40) & 0xff;
    buffer[2] = (value >> 32) & 0xff;
    buffer[3] = (value >> 24) & 0xff;
    buffer[4] = (value >> 16) & 0xff;
    buffer[5] = (value >> 8) & 0xff;
    buffer[6] = (value) & 0xff;

    return;
}


