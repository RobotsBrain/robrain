#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>



namespace NetWork {

int GetGateWayIp(char *gateway)
{
    FILE *fp = NULL;
    char buf[512];
    char *tmp = NULL;

    fp = popen("ip route", "r");
    if(fp == NULL) {
        perror("popen error");
        return -1;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL) {
        tmp = buf;

        while(*tmp && isspace(*tmp))
            ++ tmp;

        if(strncmp(tmp, "default", strlen("default")) == 0) {
            break;
        }
    }

    sscanf(buf, "%*s%*s%s", gateway);

    pclose(fp);

    return 0;
}

void DispNetInfo(const char* szDevName)
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		fprintf(stderr, "Create socket failed!errno=%d", errno);
		return;
	}

	struct ifreq ifr;
	unsigned char mac[6];
	unsigned long nIP, nNetmask, nBroadIP;
	
	strcpy(ifr.ifr_name, szDevName);

	if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
		return;
	}

	memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
	printf("\tMAC: %02x-%02x-%02x-%02x-%02x-%02x\n",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		
	strcpy(ifr.ifr_name, szDevName);

	if (ioctl(s, SIOCGIFADDR, &ifr) < 0) {
		nIP = 0;
	} else {
		nIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
	}

	printf("\tIP: %s\n", inet_ntoa(*(in_addr*)&nIP));
	strcpy(ifr.ifr_name, szDevName);

	if (ioctl(s, SIOCGIFBRDADDR, &ifr) < 0) {
		nBroadIP = 0;
	} else {
		nBroadIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
	}

	printf("\tBroadIP: %s\n", inet_ntoa(*(in_addr*)&nBroadIP));

	strcpy(ifr.ifr_name, szDevName);

	if (ioctl(s, SIOCGIFNETMASK, &ifr) < 0) {
		nNetmask = 0;
	} else {
		nNetmask = *(unsigned long*)&ifr.ifr_netmask.sa_data[2];
	}

	printf("\tNetmask: %s\n", inet_ntoa(*(in_addr*)&nNetmask));

	close(s);

	return;
}


} // end namespace


