#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <curl/curl.h>

#include "Http.h"

using namespace std;



namespace Hls {


#define USER_AGENT "Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) " \
					"AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 " \
					"Mobile/10A5355d Safari/8536.25"

CHttp::CHttp(HTTPDATACALLBACK pcbk, void *context)
: m_cbk(pcbk)
, m_context(context)
{
}

CHttp::~CHttp()
{
	m_cbk = NULL;
	m_context = NULL;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;

	CHttp *thiz = (CHttp *)userp;

	if(thiz != NULL) {
		thiz->DataCallback(contents, realsize);
	}

	return realsize;
}

void CHttp::DataCallback(void *contents, size_t size)
{
	if(m_cbk != NULL) {
		m_cbk(m_context, contents, size);
	} else {
		printf("real size: %d\n", size);
		printf("%s\n", (char *)contents);
	}

	return;
}

size_t CHttp::Request(string url)
{
	CURL *c;
	CURLcode res = CURLE_OK;

	c = curl_easy_init();

	curl_easy_setopt(c, CURLOPT_URL, url.c_str());
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(c, CURLOPT_USERAGENT, USER_AGENT);

	res = curl_easy_perform(c);
	if (res != CURLE_OK) {
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	} else {
		printf("request success!\n");
	}

	curl_easy_cleanup(c);

	return 0;
}

} // end namespace


