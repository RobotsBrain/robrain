#include "Flv.h"



CFlv::CFlv()
{
}

CFlv::~CFlv()
{
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


