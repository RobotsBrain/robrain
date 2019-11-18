#include "Http.h"
#include "StreamDownload.h"



namespace Hls {


CStreamDownload::CStreamDownload()
{

}

CStreamDownload::~CStreamDownload()
{

}

static void Parser(void *context, void *contents, size_t size)
{
    return;
}

void CStreamDownload::FetchStream(string url)
{
	CHttp *http = new CHttp(Parser, this);

	http->Request(url);
	
	delete http;
}


} // end namespace