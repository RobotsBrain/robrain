#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "VideoTag.h"



const char *frame_types[] = {
    "not defined by standard",
    "keyframe (for AVC, a seekable frame)",
    "inter frame (for AVC, a non-seekable frame)",
    "disposable inter frame (H.263 only)",
    "generated keyframe (reserved for server use only)",
    "video info/command frame"
};

const char *codec_ids[] = {
    "not defined by standard",
    "JPEG (currently unused)",
    "Sorenson H.263",
    "Screen video",
    "On2 VP6",
    "On2 VP6 with alpha channel",
    "Screen video version 2",
    "AVC"
};

const char *avc_packet_types[] = {
    "AVC sequence header",
    "AVC NALU",
    "AVC end of sequence (lower level NALU sequence ender is not required or supported)"
};

CVideoTag::CVideoTag(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlv *pFlv)
: m_pFlv(pFlv)
{
	Init(pHeader, pBuf, nLeftLen);

	u_char *pd = CTag::GetTagData();

	m_nFrameType = (pd[0] & 0xf0) >> 4;
	m_nCodecID = pd[0] & 0x0f;

	if (CTag::GetType() == 0x09 && m_nCodecID == 7) {
		ParseH264Tag();
	}
}

CVideoTag::~CVideoTag()
{
}

int CVideoTag::ParseH264Tag()
{
	u_char *pd = CTag::GetTagData();

	m_nAVCPacketType = pd[1];
	m_nCompositionTime = ShowU24(pd + 2);

	if (m_nAVCPacketType == 0) {
		ParseH264Configuration(pd);
	} else if (m_nAVCPacketType == 1) {
		m_nNaluSize = ShowU32(pd + 5);
		ParseNalu(pd);
	} else if (m_nAVCPacketType == 2) {

	}

	return 0;
}

int CVideoTag::ParseH264Configuration(u_char *pTagData)
{
	u_char *pd = pTagData;

	int nNalUnitLength = (pd[9] & 0x03) + 1;

	if(m_pFlv != NULL) {
		m_pFlv->SetNalUnitLength(nNalUnitLength);
	}

	int sps_size, pps_size;

	sps_size = ShowU16(pd + 11);
	pps_size = ShowU16(pd + 11 + (2 + sps_size) + 1);

	CTag::AllocMediaBuffer(4 + sps_size + 4 + pps_size);

	CTag::WriteMediaData((u_char *)&nH264StartCode, 0, 4);
	CTag::WriteMediaData(pd + 11 + 2, 4, sps_size);
	CTag::WriteMediaData((u_char *)&nH264StartCode, 4 + sps_size, 4);
	CTag::WriteMediaData(pd + 11 + 2 + sps_size + 2 + 1, 4 + sps_size + 4, pps_size);

	return 0;
}

int CVideoTag::ParseNalu(u_char *pTagData)
{
	u_char *pd = pTagData;
	int nOffset = 0;
	int nNalUnitLength = 0;

	if(m_pFlv != NULL) {
		m_pFlv->GetNalUnitLength(nNalUnitLength);
	}

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

	return 0;
}

void CVideoTag::PrintVideoTag()
{
	printf("  Video tag:\n");
    printf("    Frame type: %u - %s\n", m_nFrameType, frame_types[m_nFrameType]);
    printf("    Codec ID: %u - %s\n", m_nCodecID, codec_ids[m_nCodecID]);

    if(m_nCodecID == 7) {
    	printf("    AVC video tag:\n");
	    printf("      AVC packet type: %u - %s\n", m_nAVCPacketType, avc_packet_types[m_nAVCPacketType]);
	    printf("      AVC composition time: %i\n", m_nCompositionTime);
	    printf("      AVC nalu length: %i\n", m_nNaluSize);
    }

	return;
}


