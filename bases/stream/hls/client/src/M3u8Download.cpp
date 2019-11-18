#include <string.h>
#include <stdio.h>

#include "Http.h"
#include "M3u8Parser.h"
#include "M3u8Download.h"



namespace Hls {

CM3u8Download::CM3u8Download()
{
}

CM3u8Download::~CM3u8Download()
{
}

static void Parser(void *context, void *contents, size_t size)
{
	M3U8Parser parser;

    parser.parser((const char *)contents, size);

    return;
}

void CM3u8Download::FetchM3u8File(string url)
{
	CHttp *http = new CHttp(Parser, this);

	http->Request(url);
	
	delete http;
}

} // end namespace