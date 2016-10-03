#include "DumpH264File.h"



CDumpH264File::CDumpH264File()
: m_fp(NULL)
{

}

CDumpH264File::~CDumpH264File()
{

}

void CDumpH264File::Open(std::string filename)
{
	if ((m_fp = fopen(filename.c_str(), "wa+")) == NULL) {
		printf("open file error!\n");
		return;
	}

	return;
}

void CDumpH264File::Close()
{
	if (m_fp != NULL) {
		fclose(m_fp);
		m_fp = NULL;
	}

	return;
}

void CDumpH264File::Write(char *buf, int length)
{
	fwrite(buf, length, 1, m_fp);
}


