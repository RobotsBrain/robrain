#include <string.h>

#include "FlvParser.h"
#include "Tag.h"


CTag::CTag()
: m_pTagHeader(NULL)
, m_pTagData(NULL)
, m_pMedia(NULL)
, m_nMediaLen(0)
, m_pFlvParser(NULL)
{
}

CTag::~CTag()
{
	delete[] m_pTagHeader;
	delete[] m_pTagData;
	delete[] m_pMedia;
}

void CTag::Init(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlvParser *pParser)
{
	memcpy(&m_header, pHeader, sizeof(TagHeader));

	m_pTagHeader = new u_char[11];
	memcpy(m_pTagHeader, pBuf, 11);

	m_pTagData = new u_char[m_header.nDataSize];
	memcpy(m_pTagData, pBuf + 11, m_header.nDataSize);

	m_pFlvParser = pParser;

	return;
}

u_char *CTag::GetTagHeader()
{
	return m_pTagHeader;
}

u_char *CTag::GetTagData()
{
	return m_pTagData;
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

void CTag::SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig)
{
	if(m_pFlvParser != NULL) {
		m_pFlvParser->SetAudioSpecificConfig(aacProfile, sampleRateIndex, channelConfig);
	}
}

void CTag::GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig)
{
	if(m_pFlvParser != NULL) {
		m_pFlvParser->GetAudioSpecificConfig(aacProfile, sampleRateIndex, channelConfig);
	}
}

void CTag::SetNalUnitLength(int len)
{
	if(m_pFlvParser != NULL) {
		m_pFlvParser->SetNalUnitLength(len);
	}
}

void CTag::GetNalUnitLength(int &len)
{
	if(m_pFlvParser != NULL) {
		m_pFlvParser->GetNalUnitLength(len);
	}
}

