#pragma once

#include "Video.h"


int WriteAAC2Ts(unsigned char *pData, int iDataSize);

int WriteH2642Ts(unsigned int framerate, unsigned char *pData, int iDataSize);

