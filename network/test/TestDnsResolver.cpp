#include <string.h>
#include <stdio.h>

#include "DnsResolver.h"



int main(int argc, char **argv) 
{ 
    if(argc != 2) {
    	printf("please input host name!\n");
    	return -1;
    }
    
    NetWork::HostByName(argv[1]);
    
    NetWork::AdnsAnalysis(argv[1], 20);

    NetWork::GetAddressInfo(argv[1]);

    return 0; 
}


