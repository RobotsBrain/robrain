#include "DumpFile.h"



namespace Base {

CDumpFile::CDumpFile()
: m_fp(NULL)
{

}

CDumpFile::~CDumpFile()
{

}

void CDumpFile::Open(std::string filename)
{
	if ((m_fp = fopen(filename.c_str(), "wa+")) == NULL) {
		printf("open file error!\n");
		return;
	}

	return;
}

void CDumpFile::Close()
{
	if (m_fp != NULL) {
		fclose(m_fp);
		m_fp = NULL;
	}

	return;
}

void CDumpFile::Write(char *buf, int length)
{
	fwrite(buf, length, 1, m_fp);
}

} // end namespace Base
