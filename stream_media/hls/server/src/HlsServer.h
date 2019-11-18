#ifndef __HLSSERVER_H__
#define __HLSSERVER_H__

#include <event.h>
#include <evhttp.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <string>

#ifdef _EVENT_HAVE_NETINET_IN_H
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#endif



namespace Hls {

class CHlsServer
{
public:
	CHlsServer(std::string host, int port, std::string rootdir);
	~CHlsServer();

	bool Start();
	bool Stop();

	void SendDocument(struct evhttp_request *req);
	
private:
	bool ExtractDisplayInfo(struct evhttp_bound_socket *handle);

private:
	std::string m_host;
	int m_port;
	std::string m_rootdir;
	std::string m_uriroot;
};

} // end namespace

#endif