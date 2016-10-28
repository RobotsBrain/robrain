#include <stdlib.h>
#include <stdio.h>

#include "RtspStream.h"



int main(int argc, char **argv)  
{
    CRTSPStream rtspSender;

    bool bRet = rtspSender.Init();

    rtspSender.SendH264File("test.h264");

    return 0;   
}


