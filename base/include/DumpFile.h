#ifndef __DUMPFILE_H__
#define __DUMPFILE_H__

#include <stdint.h>
#include <stdio.h>
#include <string>



class CDumpFile
{
public:
	CDumpFile();
	~CDumpFile();
	
public:
	void Open(std::string filename);
	void Close();
	void Write(char *buf, int length);

private:
	FILE *m_fp;
};

#endif


