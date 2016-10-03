#ifndef __DUMPH264FILE_H__
#define __DUMPH264FILE_H__

#include <stdint.h>
#include <stdio.h>
#include <string>



class CDumpH264File
{
public:
	CDumpH264File();
	~CDumpH264File();
	
public:
	void Open(std::string filename);
	void Close();
	void Write(char *buf, int length);

private:
	FILE *m_fp;
};

#endif


