#ifndef __LIVERTMPSTREAM_H__
#define __LIVERTMPSTREAM_H__



class CLiveRtmpStream
{
public:
	CLiveRtmpStream();
	~CLiveRtmpStream();

	bool Start();
	bool Stop();
	
};

#endif