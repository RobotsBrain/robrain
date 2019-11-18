#ifndef __HTTP_H__
#define __HTTP_H__

#include <unistd.h>
#include <string>


namespace Hls {

typedef void (*HTTPDATACALLBACK)(void *context, void *contents, size_t size);

class CHttp
{
public:
	CHttp(HTTPDATACALLBACK pcbk = NULL, void *context = NULL);
	~CHttp();

	void DataCallback(void *contents, size_t size);
	size_t Request(std::string url);

private:
	HTTPDATACALLBACK 	m_cbk;
	void 				*m_context;
};

} // end namespace

#endif