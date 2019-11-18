#ifndef __FLVDEMUX_H__
#define __FLVDEMUX_H__

#include <stdint.h>
#include <string>
#include <vector>

#include "flv/IFlvDemux.h"
#include "flv/Flv.h"


namespace Flv {


class CFlvDemux : public IFlvDemux
{
public:
	CFlvDemux();
	~CFlvDemux();

	virtual void Destroy();
	virtual void SetCallback(IAVDataCallback *pcbk, void *context);
	virtual int Parser(u_char *pBuf, int nBufSize, int &nUsedLen);

private:
	int ParserTag(u_char *pBuf, int nLeftLen);

	void ParserVideoTag(TagHeader header, u_char *pBuf);
	int ParseNalu(TagHeader header, u_char *pTagData);
	int ParseH264Configuration(u_char *pTagData);

	void ParserAudioTag(TagHeader header, u_char *pBuf);

private:
	bool 	m_bFlvHeader;
	int 	m_nNalUnitLength;

	void 			*m_context;
	IAVDataCallback *m_pAvCbk;
};

} // end namespace

#endif
