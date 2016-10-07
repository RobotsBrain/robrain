#ifndef __AUDIOMANAGE_H__
#define __AUDIOMANAGE_H__

#include <alsa/asoundlib.h>

#include "ThreadLoop.h"



class CAudioManage: public Base::CThreadLoop
{
public:
	CAudioManage();
	~CAudioManage();
	
	bool Start();
	bool Stop();

private:
	void PrintHwParams(snd_pcm_hw_params_t *params);
	virtual void EventHandleLoop();

private:
	snd_pcm_t 			*m_handle;  
	snd_pcm_hw_params_t *m_params;   
	snd_pcm_uframes_t 	m_frames;
};

#endif


