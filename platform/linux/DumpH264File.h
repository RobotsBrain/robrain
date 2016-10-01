#ifndef __DUMPH264FILE_H__
#define __DUMPH264FILE_H__

#include <stdio.h>
#include <string>



class CDumpH264File
{
public:
	CDumpH264File();
	~CDumpH264File();
	
public:
	void Open(std::string filename);
	void Stop();
	void Write(uint8_t *buf, int length);

private:
	File *m_fp;
};

#endif


