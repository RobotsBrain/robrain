#include "Common.h"
#include "AudioTag.h"



CAudioTag::CAudioTag(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlvParser *pParser)
{
	Init(pHeader, pBuf, nLeftLen, pParser);

	u_char *pd = CTag::GetTagData();

	m_nSoundFormat = (pd[0] & 0xf0) >> 4;
	m_nSoundRate = (pd[0] & 0x0c) >> 2;
	m_nSoundSize = (pd[0] & 0x02) >> 1;
	m_nSoundType = (pd[0] & 0x01);

	if (m_nSoundFormat == 10) {	// AAC
		ParseAACTag();
	}
}

int CAudioTag::ParseAACTag()
{
	u_char *pd = CTag::GetTagData();
	int nAACPacketType = pd[1];

	if (nAACPacketType == 0) {
		ParseAudioSpecificConfig();
	} else if (nAACPacketType == 1) {
		ParseRawAAC(pd);
	}

	return 1;
}

int CAudioTag::ParseAudioSpecificConfig()
{
	u_char *pd = CTag::GetTagData();

	int aacProfile = ((pd[2] & 0xf8) >> 3) - 1;
	int sampleRateIndex = ((pd[2] & 0x07) << 1) | (pd[3] >> 7);
	int channelConfig = (pd[3] >> 3) & 0x0f;

	CTag::SetAudioSpecificConfig(aacProfile, sampleRateIndex, channelConfig);

	return 1;
}

int CAudioTag::ParseRawAAC(u_char *pTagData)
{
	uint64_t bits = 0;
	int aacProfile = 0;
	int sampleRateIndex = 0;
	int channelConfig = 0;
	int dataSize = CTag::GetDataSize() - 2;

	CTag::GetAudioSpecificConfig(aacProfile, sampleRateIndex, channelConfig);
	CTag::AllocMediaBuffer(7 + dataSize);

	WriteU64(bits, 12, 0xFFF);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 2, 0);
	WriteU64(bits, 1, 1);
	WriteU64(bits, 2, aacProfile);
	WriteU64(bits, 4, sampleRateIndex);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 3, channelConfig);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 13, 7 + dataSize);
	WriteU64(bits, 11, 0x7FF);
	WriteU64(bits, 2, 0);

	u_char p64[8];

	p64[0] = (u_char)(bits >> 56);
	p64[1] = (u_char)(bits >> 48);
	p64[2] = (u_char)(bits >> 40);
	p64[3] = (u_char)(bits >> 32);
	p64[4] = (u_char)(bits >> 24);
	p64[5] = (u_char)(bits >> 16);
	p64[6] = (u_char)(bits >> 8);
	p64[7] = (u_char)(bits);

	CTag::WriteMediaData(p64 + 1, 0, 7);
	CTag::WriteMediaData(pTagData + 2, 7, dataSize);

	return 1;
}


