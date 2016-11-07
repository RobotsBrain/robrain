#include <string.h>
#include <stdio.h>

#include "FlvParser.h"
#include "Tag.h"


CTag::CTag()
: m_pTagHeader(NULL)
, m_pTagData(NULL)
, m_pMedia(NULL)
, m_nMediaLen(0)
{
}

CTag::~CTag()
{
	delete[] m_pTagHeader;
	delete[] m_pTagData;
	delete[] m_pMedia;
}

void CTag::Init(TagHeader *pHeader, u_char *pBuf, int nLeftLen)
{
	memcpy(&m_header, pHeader, sizeof(TagHeader));

	m_pTagHeader = new u_char[11];
	memcpy(m_pTagHeader, pBuf, 11);

	m_pTagData = new u_char[m_header.nDataSize];
	memcpy(m_pTagData, pBuf + 11, m_header.nDataSize);

	return;
}

u_char *CTag::GetTagHeaderData()
{
	return m_pTagHeader;
}

u_char *CTag::GetTagData()
{
	return m_pTagData;
}

TagHeader *CTag::GetTagHeader()
{
	return &m_header;
}

int CTag::GetType()
{
	return m_header.nType;
}

uint32_t CTag::GetDataSize()
{
	return m_header.nDataSize;
}

void CTag::AllocMediaBuffer(int datasize)
{
	m_pMedia = new u_char[datasize];

	return;
}

u_char *CTag::GetMedia()
{
	return m_pMedia;
}

int CTag::GetMediaLen()
{
	return m_nMediaLen;
}

void CTag::WriteMediaData(u_char *src, int offset, int size)
{
	memcpy(m_pMedia + offset, src, size);

	m_nMediaLen += size;

	return;
}

void CTag::PrintTagHeader()
{
	printf("\nTag type: %u - ", m_header.nType);
            
    if(m_header.nType == 0x08) {
    	printf("Audio data\n");
    } else if (m_header.nType == 0x09) {
    	printf("Video data\n");
    } else if (m_header.nType == 0x12) {
    	printf("Script data object\n");
    }

    printf("  Data size: %lu\n", (unsigned long)m_header.nDataSize);
    printf("  Timestamp: %lu\n", (unsigned long)m_header.nTimeStamp);
    printf("  Timestamp extended: %u\n", m_header.nTSEx);
    printf("  StreamID: %lu\n", (unsigned long)m_header.nStreamID);

    return;
}


