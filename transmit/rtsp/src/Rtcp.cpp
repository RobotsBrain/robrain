#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "Rtcp.h"



namespace Rtsp {

int rtcp_send_packet(struct rtcp_pkt &pkt, const char *host_name)
{
	RTCP_header hdr;	
	RTCP_header_SDES hdr_sdes;
	unsigned int pkt_size = 0, hdr_s = 0, hdr_sdes_s, local_ssrc, name_s;
	
	pkt.comm.version = 2;
	pkt.comm.padding = 0;
	pkt.comm.count = 1;
	pkt.comm.pt = SDES;
	hdr_s = sizeof(hdr);
	hdr_sdes_s = sizeof(hdr_sdes);
	name_s = strlen(host_name);
	//pkt_size=hdr_s+hdr_sdes_s+name_s;
	pkt_size = sizeof(rtcp_pkt) + name_s;
	pkt.comm.length = htons((pkt_size >> 2) -1);
	local_ssrc = Random32(0);
	pkt.sdec.ssrc = htonl(local_ssrc);    
	pkt.sdec.attr_name = CNAME;	// 1=CNAME
	pkt.sdec.len = name_s;
	memcpy(pkt.sdec.name, host_name, name_s);

	return pkt_size;
}


} // end namespace


