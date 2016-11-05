#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "VideoTag.h"



CVideoTag::CVideoTag(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlvParser *pParser)
{
	Init(pHeader, pBuf, nLeftLen, pParser);

	u_char *pd = CTag::GetTagData();

	m_nFrameType = (pd[0] & 0xf0) >> 4;
	m_nCodecID = pd[0] & 0x0f;

	if (CTag::GetType() == 0x09 && m_nCodecID == 7) {
		ParseH264Tag();
	}
}

int CVideoTag::ParseH264Tag()
{
	u_char *pd = CTag::GetTagData();
	int nAVCPacketType = pd[1];
	int nCompositionTime = ShowU24(pd + 2);

	if (nAVCPacketType == 0) {
		ParseH264Configuration(pd);
	} else if (nAVCPacketType == 1) {
		ParseNalu(pd);
	} else if (nAVCPacketType == 2) {

	}

	return 1;
}

int CVideoTag::ParseH264Configuration(u_char *pTagData)
{
	u_char *pd = pTagData;

	int nNalUnitLength = (pd[9] & 0x03) + 1;

	CTag::SetNalUnitLength(nNalUnitLength);

	int sps_size, pps_size;

	sps_size = ShowU16(pd + 11);
	pps_size = ShowU16(pd + 11 + (2 + sps_size) + 1);

	CTag::AllocMediaBuffer(4 + sps_size + 4 + pps_size);

	CTag::WriteMediaData((u_char *)&nH264StartCode, 0, 4);
	CTag::WriteMediaData(pd + 11 + 2, 4, sps_size);
	CTag::WriteMediaData((u_char *)&nH264StartCode, 4 + sps_size, 4);
	CTag::WriteMediaData(pd + 11 + 2 + sps_size + 2 + 1, 4 + sps_size + 4, pps_size);

	return 1;
}

int CVideoTag::ParseNalu(u_char *pTagData)
{
	u_char *pd = pTagData;
	int nOffset = 0;
	int nNalUnitLength = 0;

	CTag::GetNalUnitLength(nNalUnitLength);;

	CTag::AllocMediaBuffer(CTag::GetDataSize() + 10);

	nOffset = 5;

	while (1) {
		if (nOffset >= CTag::GetDataSize()) {
			break;
		}

		int nNaluLen;

		switch (nNalUnitLength) {
		case 4:
			nNaluLen = ShowU32(pd + nOffset);
			break;

		case 3:
			nNaluLen = ShowU24(pd + nOffset);
			break;

		case 2:
			nNaluLen = ShowU16(pd + nOffset);
			break;

		default:
			nNaluLen = ShowU8(pd + nOffset);
			break;
		}

		CTag::WriteMediaData((u_char *)&nH264StartCode, CTag::GetMediaLen(), 4);
		CTag::WriteMediaData(pd + nOffset + nNalUnitLength, CTag::GetMediaLen(), nNaluLen);

		nOffset += (nNalUnitLength + nNaluLen);
	}

	return 1;
}


