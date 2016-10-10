#include <string.h>
#include <stdio.h>

#include "LnwDetail.h"



int main(int argc, char **argv)
{
	char gateway[32] = {0};

	NetWork::GetGateWayIp(gateway);

	printf("gateway ip addr: %s\n", gateway);

	NetWork::DispNetInfo("eth0");

	return 0;
}


