#include <stdio.h>

#include "Common.h"
#include "AudioTag.h"



const char *sound_formats[] = {
    "Linear PCM, platform endian",
    "ADPCM",
    "MP3",
    "Linear PCM, little endian",
    "Nellymoser 16-kHz mono",
    "Nellymoser 8-kHz mono",
    "Nellymoser",
    "G.711 A-law logarithmic PCM",
    "G.711 mu-law logarithmic PCM",
    "not defined by standard",
    "AAC",
    "Speex",
    "not defined by standard",
    "not defined by standard",
    "MP3 8-Khz",
    "Device-specific sound"
};

const char *sound_rates[] = {
    "5.5-Khz",
    "11-Khz",
    "22-Khz",
    "44-Khz"
};

const char *sound_sizes[] = {
    "8 bit",
    "16 bit"
};

const char *sound_types[] = {
    "Mono",
    "Stereo"
};

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

CAudioTag::~CAudioTag()
{
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

	return 0;
}

int CAudioTag::ParseAudioSpecificConfig()
{
	u_char *pd = CTag::GetTagData();

	int aacProfile = ((pd[2] & 0xf8) >> 3) - 1;
	int sampleRateIndex = ((pd[2] & 0x07) << 1) | (pd[3] >> 7);
	int channelConfig = (pd[3] >> 3) & 0x0f;

	CTag::SetAudioSpecificConfig(aacProfile, sampleRateIndex, channelConfig);

	return 0;
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

	return 0;
}

void CAudioTag::PrintAudioTag()
{
	printf("  Audio tag:\n");
    printf("    Sound format: %u - %s\n", m_nSoundFormat, sound_formats[m_nSoundFormat]);
    printf("    Sound rate: %u - %s\n", m_nSoundRate, sound_rates[m_nSoundRate]);

    printf("    Sound size: %u - %s\n", m_nSoundSize, sound_sizes[m_nSoundSize]);
    printf("    Sound type: %u - %s\n", m_nSoundType, sound_types[m_nSoundType]);
}


