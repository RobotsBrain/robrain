#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Adts.h"

using namespace std;



static void print_binary(u_int64_t value) 
{
    int index = 0;
    int binary_count = sizeof(u_int64_t) * 8;

    while (binary_count-- > 0) {
        u_int64_t tmp = value >> binary_count;
        if ((tmp & 0x01) == 1) {
            printf("1");
        } else {
            printf("0");
        }
        
        if (++index % 8 == 0) {
            printf(" ");
        }
    }
    
    printf("==> %lld \n", value);
}

static void print_fixed_header(const AdtsFixedFeader *header) 
{
    printf("\nADTS fixed header details:\n");
    printf("    syncword: 0x%02X\n", header->syncword);
    printf("    id: %d\n", header->id);
    printf("    layer: %d\n", header->layer);
    printf("    protection_absent: %d\n", header->protection_absent);
    printf("    profile: %d\n", header->profile);
    printf("    sampling_frequency_index: %d\n", header->sampling_frequency_index);
    printf("    private_bit: %d\n", header->private_bit);
    printf("    channel_configuration: %d\n", header->channel_configuration);
    printf("    original_copy: %d\n", header->original_copy);
    printf("    home: %d\n", header->home);

    return;
}

static void print_variable_header(const AdtsVariableHeader *header) 
{
    printf("\nADTS variable header details:\n");
    printf("    copyright_identification_bit: %d\n", header->copyright_identification_bit);
    printf("    copyright_identification_start: %d\n", header->copyright_identification_start);
    printf("    aac_frame_length: %d\n", header->aac_frame_length);
    printf("    adts_buffer_fullness: %d\n", header->adts_buffer_fullness);
    printf("    number_of_raw_data_blocks_in_frame: %d\n", header->number_of_raw_data_blocks_in_frame);
    printf("------------------------------------------\n");

    return;
}

void Simulation()
{
    unsigned char adts[7];
    AdtsFixedFeader fixed_header;
    AdtsVariableHeader variable_header;
    
    memset(&fixed_header, 0, sizeof(AdtsFixedFeader));
    memset(&variable_header, 0, sizeof(AdtsVariableHeader));

    SetFixedHeader(&fixed_header);
    SetVariableHeader(&variable_header, 100);
    
    AdtsHeader2Char(&fixed_header, &variable_header, adts);
    
    print_fixed_header(&fixed_header);
    print_variable_header(&variable_header);
    
 
    memset(&fixed_header, 0, sizeof(AdtsFixedFeader));
    memset(&variable_header, 0, sizeof(AdtsVariableHeader));
    GetFixedHeader(adts, &fixed_header);
    GetVariableHeader(adts, &variable_header);

    print_fixed_header(&fixed_header);
    print_variable_header(&variable_header);

    return;   
}

int GetOneAACFrame(u_char *pBufIn, int nInSize, u_char *pAACFrame, int &nAACFrameSize)
{
    unsigned char *p = pBufIn;

    if (nInSize <= 7) {
        return 0;
    }

    int nFrameSize = ((p[3] & 0x3) << 11) + (p[4] << 3) + (p[5] >> 5);
    if (nInSize < nFrameSize) {
        return 0;
    }

    nAACFrameSize = nFrameSize;
    memcpy(pAACFrame, pBufIn, nFrameSize);

    return 1;
}

int ParserAacFile(string audiofile)
{
    fstream fFile;
    int nFileSize;
    u_char *pIn = NULL;
    u_char *pOut = NULL;
    int nOffset = 0;
    int nAACFrameSize = 0;

    fFile.open(audiofile.c_str(), ios::binary | ios::in);

    fFile.seekg(0, ios::end);
    nFileSize = fFile.tellg();

    fFile.seekg(0, ios_base::beg);

    pIn = new u_char[nFileSize];
    pOut = new u_char[nFileSize];

    fFile.read((char *)pIn, nFileSize);

    while(1) {
        if (GetOneAACFrame(pIn + nOffset, nFileSize - nOffset, pOut, nAACFrameSize) == 0) {
            return -1;
        }

        AdtsFixedFeader fixed_header;
        AdtsVariableHeader variable_header;

        GetFixedHeader(pOut, &fixed_header);
        GetVariableHeader(pOut, &variable_header);

        print_fixed_header(&fixed_header);
        print_variable_header(&variable_header);

        nOffset += nAACFrameSize;

        if (nOffset >= nFileSize - 4) {
            return -1;
        }
    }
    
    delete pIn;
    delete pOut;

    fFile.close();

    return 0;
}

int main(int argc, char **argv) 
{
    int ch;
    string audiofile;

    while ((ch = getopt(argc, argv, "a:")) != -1) {

        switch (ch) {
        case 'a':
            audiofile = optarg;
            break;

        default:
            break;
        }
    }

    if(audiofile.size()) {
        ParserAacFile(audiofile);
    } else {
        Simulation();
    }
    
    return 0;
}


