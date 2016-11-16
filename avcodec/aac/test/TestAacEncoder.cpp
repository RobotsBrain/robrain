#include <faac.h>
#include <stdio.h>



int main(int argc, char* argv[])
{
    unsigned long nSampleRate = 11025;  // 采样率
    unsigned int nChannels = 1;         // 声道数
    unsigned int nPCMBitSize = 16;      // 单样本位数
    unsigned long nInputSamples = 0;
    unsigned long nMaxOutputBytes = 0;
    int nRet;
    faacEncHandle hEncoder;
    faacEncConfigurationPtr pConfiguration; 

    int nBytesRead;
    int nPCMBufferSize;
    unsigned char* pbPCMBuffer;
    unsigned char* pbAACBuffer;

    FILE* fpIn; // WAV file for input
    FILE* fpOut; // AAC file for output

    fpIn = fopen("in.wav", "rb");
    fpOut = fopen("out.aac", "wb");

    // (1) Open FAAC engine
    hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
    if(hEncoder == NULL) {
        printf("[ERROR] Failed to call faacEncOpen()\n");
        return -1;
    }

    nPCMBufferSize = nInputSamples * nPCMBitSize / 8;
    pbPCMBuffer = new unsigned char [nPCMBufferSize];
    pbAACBuffer = new unsigned char [nMaxOutputBytes];

    // (2.1) Get current encoding configuration
    pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;

    // (2.2) Set encoding configuration
    nRet = faacEncSetConfiguration(hEncoder, pConfiguration);

    for(int i = 0; 1; i++) {
        // 读入的实际字节数，最大不会超过nPCMBufferSize，一般只有读到文件尾时才不是这个值
        nBytesRead = fread(pbPCMBuffer, 1, nPCMBufferSize, fpIn);

        // 输入样本数，用实际读入字节数计算，一般只有读到文件尾时才不是nPCMBufferSize/(nPCMBitSize/8);
        nInputSamples = nBytesRead / (nPCMBitSize / 8);

        // (3) Encode
        nRet = faacEncEncode(
        hEncoder, (int*) pbPCMBuffer, nInputSamples, pbAACBuffer, nMaxOutputBytes);

        fwrite(pbAACBuffer, 1, nRet, fpOut);

        printf("%d: faacEncEncode returns %d\n", i, nRet);

        if(nBytesRead <= 0) {
            break;
        }
    }

    // (4) Close FAAC engine
    nRet = faacEncClose(hEncoder);

    delete[] pbPCMBuffer;
    delete[] pbAACBuffer;
    fclose(fpIn);
    fclose(fpOut);

    //getchar();

    return 0;
}


