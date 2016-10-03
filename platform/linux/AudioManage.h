#ifndef __AUDIOMANAGE_H__
#define __AUDIOMANAGE_H__

#include <pthread.h>
#include <alsa/asoundlib.h>

#include "DumpFile.h"



class CAudioManage
{
public:
	CAudioManage();
	~CAudioManage();
	
	void Start();
	void Stop();

private:
	void PrintHwParams(snd_pcm_hw_params_t *params);
	void ReadAndEncodeFrame();

	static void *ThreadProc(void *argv);

private:
	pthread_t 			m_tid;  
    pthread_attr_t 		m_attr;

	snd_pcm_t 			*m_handle;  
	snd_pcm_hw_params_t *m_params;   
	snd_pcm_uframes_t 	m_frames;

	Base::CDumpFile		m_pcmfile;
};

#endif


