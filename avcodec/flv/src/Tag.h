#ifndef __TAG_H__
#define __TAG_H__

#include <sys/types.h>
#include <stdint.h>



class CFlv;

typedef struct {
	int nType;
	int nDataSize;
	int nTimeStamp;
	int nTSEx;
	int nStreamID;

	uint32_t nTotalTS;
} TagHeader;

class CTag {
public:
	CTag();
	virtual ~CTag();

	void Init(TagHeader *pHeader, u_char *pBuf, int nLeftLen, CFlv *pParser);

	u_char *GetTagHeaderData();

	void PrintTagHeader();

	TagHeader *GetTagHeader();
	int GetType();
	u_char *GetTagData();
	uint32_t GetDataSize();

	void AllocMediaBuffer(int datasize);
	void WriteMediaData(u_char *src, int offset, int size);

	u_char *GetMedia();
	int GetMediaLen();

	void SetAudioSpecificConfig(int aacProfile, int sampleRateIndex, int channelConfig);
	void GetAudioSpecificConfig(int &aacProfile, int &sampleRateIndex, int &channelConfig);

	void SetNalUnitLength(int len);
	void GetNalUnitLength(int &len);

private:
	TagHeader 	m_header;
	u_char 		*m_pTagHeader;
	u_char 		*m_pTagData;
	u_char 		*m_pMedia;
	int 		m_nMediaLen;
	CFlv	    *m_pFlvParser;
};

#endif