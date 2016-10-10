#include <sys/errno.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>

#include "adns.h"


using namespace std;

namespace NetWork {


int HostByName(string name)
{
    int i, sockfd;
    struct hostent *host;
    struct sockaddr_in serv_addr;

    if ((host = gethostbyname(name.c_str())) == NULL) {
        printf("gethostbyname error\n");
        return -1;
    }

    printf("official name: %s, address length: %d bytes\n", host->h_name, host->h_length);

    printf("host name alias: \n");
    for (i = 0; host->h_aliases[i]; i++) {
        printf("%s\n", host->h_aliases[i]);
    }

    for (i = 0; host->h_addr_list[i]; i++) {
        /* h_addr_list[i]指向in_addr类型 */
        serv_addr.sin_addr = *((struct in_addr *)host->h_addr_list[i]);
        const char *ip = inet_ntoa(serv_addr.sin_addr);
        printf("host by name: %s\n", ip);
    }

    return 0;
}

int GetAddressInfo(string hostname)
{
    struct addrinfo *answer = NULL, *curr = NULL;
    struct addrinfo hint;
    struct sockaddr_in *serv_addr;

    bzero(&hint, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(hostname.c_str(), NULL, &hint, &answer);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return -1;
    }

    for (curr = answer; curr != NULL; curr = curr->ai_next) {
        serv_addr = (struct sockaddr_in *)(curr->ai_addr);
        printf("get addr info: %s\n", inet_ntoa(serv_addr->sin_addr));
    }

    freeaddrinfo(answer);

    return 0;
}

int AdnsAnalysis(string host, int looptime)
{
    int count = 0;
    int adns_cname = 0;
    adns_state ads; 
    adns_initflags flags;
    adns_query quer = NULL;  
    flags = (adns_initflags)(adns_if_nosigpipe | adns_if_noerrprint); 
    
    adns_init(&ads, flags, NULL); 
    adns_submit(ads, host.c_str(), (adns_rrtype)adns_r_a, (adns_queryflags)0, NULL, &quer);  
 
    do {     
        adns_answer *ans = NULL; 

        int res = adns_check(ads, &quer, &ans, NULL); 
        if(res == 0) {       
            if (ans->status == adns_s_prohibitedcname) { 
                char cname[128]; 
                strncpy(cname, ans->cname, 127); 
                cname[strlen(ans->cname)] = '\0';       
                adns_query quer = NULL; 
                adns_submit(ads, cname, (adns_rrtype)adns_r_addr, (adns_queryflags)0, 
                            NULL, &quer);       
                adns_cname = 1; 
            } else { 
#if 1
                //resolve IPv4 address                 
                if(adns_cname) {
                    printf("ip: %s\n", ans->status == adns_s_ok ? 
                            inet_ntoa(ans->rrs.addr->addr.inet.sin_addr) : "no"); 
                } else {
                    printf("ip: %s\n", ans->status == adns_s_ok ? 
                            inet_ntoa(*(ans->rrs.inaddr)) : "no");             
                }
#else
                //resolve IPv6 address
                if(adns_cname){
                    if(ans->status == adns_s_ok) {
                        char buf[INET6_ADDRSTRLEN];
                        inet_ntop(AF_INET6, &ans->rrs.addr->addr.inet6.sin6_addr, 
                                    buf, sizeof(buf));
                        printf("ip: %s\n", buf);
                    } else {
                        printf("no\n");
                    }
                } else {
                    if(ans->status == adns_s_ok) {
                        char buf[INET6_ADDRSTRLEN];
                        inet_ntop(AF_INET6, ans->rrs.in6addr, buf, sizeof(buf));
                        printf("ip: %s\n", buf);
                    } else {
                        printf("no\n");
                    }
                }
#endif
                adns_finish(ads); 
                break; 
            }
        } else if (res == ESRCH || res == EAGAIN) { 
            sleep(1); 
        } else { 
            printf("host(%s) is err!\n", host.c_str()); 
        } 
    } while(++count < looptime);
    
    return 0; 
}


} // end namespace


