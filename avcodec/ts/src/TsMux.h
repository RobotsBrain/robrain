



typedef void (*STREAMCALLBACK)(unsigned char *data, int len);

void SetCallback(STREAMCALLBACK pcbk);

int WriteAAC2Ts(unsigned char *pData, int iDataSize);

int WriteH2642Ts(unsigned int framerate, unsigned char *pData, int iDataSize);

