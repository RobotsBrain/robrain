#include <string.h>
#include <stdio.h>

#include "Flv.h"



CFlv::CFlv()
{
}

CFlv::~CFlv()
{
	DestroyTag();
	DestroyFlvHeader();
}

void CFlv::WriteHeaderData(FlvHeader Header, u_char *pBuf)
{
	m_pFlvHeader.nVersion = Header.nVersion;
	m_pFlvHeader.bHaveAudio = Header.bHaveAudio;
	m_pFlvHeader.bHaveVideo = Header.bHaveVideo;
	m_pFlvHeader.nHeadSize = Header.nHeadSize;

	m_pFlvHeader.pFlvHeader = new u_char[Header.nHeadSize];
	memcpy(m_pFlvHeader.pFlvHeader, pBuf, Header.nHeadSize);

	return;
}

void CFlv::DestroyFlvHeader()
{
	if (m_pFlvHeader.pFlvHeader != NULL) {
		delete m_pFlvHeader.pFlvHeader;
	}

	return;
}

void CFlv::SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig)
{
	m_aacProfile = aacProfile;
	m_sampleRateIndex = sampleRateIndex;
	m_channelConfig = channelConfig;
}

void CFlv::GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig)
{
	aacProfile = m_aacProfile;
	sampleRateIndex = m_sampleRateIndex;
	channelConfig = m_channelConfig;
}

void CFlv::SetNalUnitLength(int len)
{
	m_nNalUnitLength = len;
}

void CFlv::GetNalUnitLength(int &len)
{
	len = m_nNalUnitLength;
}

void CFlv::WriteTag(CTag *pTag)
{
	m_vpTag.push_back(pTag);

	return;
}

void CFlv::DestroyTag()
{
	for (int i = 0; i < m_vpTag.size(); i++) {
		delete m_vpTag[i];
	}

	return;
}

void CFlv::GetTags(std::vector <CTag *> &vpTag)
{
	vpTag = m_vpTag;

	return;
}

void CFlv::GetFlvHeader(FlvHeader &flvHeader)
{
    memcpy(&flvHeader, &m_pFlvHeader, sizeof(FlvHeader));

	return;
}

void CFlv::PrintFlvHeader()
{
	printf("FLV file version %u\n", m_pFlvHeader.nVersion);

    printf("  Contains audio tags: ");

    if (m_pFlvHeader.bHaveAudio) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }

    printf("  Contains video tags: ");

    if (m_pFlvHeader.bHaveVideo) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }

    printf("  Data offset: %lu\n", (unsigned long)m_pFlvHeader.nHeadSize);

    return;
}


