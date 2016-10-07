#include "AudioManage.h"



CAudioManage::CAudioManage()
: Base::CThreadLoop("AudioManage")
{
}

CAudioManage::~CAudioManage()
{
}

bool CAudioManage::Start()
{
	snd_pcm_t *handle = NULL;  
	snd_pcm_hw_params_t *params = NULL;  
	unsigned int val;  
	int dir, rc = 0;  
	snd_pcm_uframes_t frames;

	/* Open PCM device for recording (capture). */  
	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);  
	if (rc < 0) {  
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));  
		return false;
	}

	/* Allocate a hardware parameters object. */  
	snd_pcm_hw_params_alloca(&params);  
	/* Fill it in with default values. */  
	snd_pcm_hw_params_any(handle, params);  
	/* Set the desired hardware parameters. */  
	/* Interleaved mode */  
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);  
	/* Signed 16-bit little-endian format */  
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);  
	/* Two channels (stereo) */  
	snd_pcm_hw_params_set_channels(handle, params, 2);  
	/* 44100 bits/second sampling rate (CD quality) */  
	val = 44100;  
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);  
	/* Set period size to 32 frames. */  
	frames = 32;  
	snd_pcm_hw_params_set_period_size_near(handle,  params, &frames, &dir);  
	/* Write the parameters to the driver */  
	rc = snd_pcm_hw_params(handle, params);  
	if (rc < 0) {  
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));  
		return false;  
	}  
	/* Use a buffer large enough to hold one period */  
	snd_pcm_hw_params_get_period_size(params,  &frames, &dir);

	PrintHwParams(params);

	m_handle = handle;
	m_params = params;
	m_frames = frames;

	return StartThread();
}

bool CAudioManage::Stop()
{
	snd_pcm_drain(m_handle);
	snd_pcm_close(m_handle);

	return StartThread();
}

void CAudioManage::EventHandleLoop()
{
	int rc = 0;
	int size = m_frames * 4; /* 2 bytes/sample, 2 channels */
	char *buffer = (char *)malloc(size);

	m_pcmfile.Open("audio.pcm");

	while (1) {
		rc = snd_pcm_readi(m_handle, buffer, m_frames);
		if (rc == -EPIPE) {
			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred\n");
			snd_pcm_prepare(m_handle);
		} else if (rc < 0) {
			fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
		} else if (rc != (int)m_frames) {
			fprintf(stderr, "short read, read %d frames\n", rc);
		}

		m_pcmfile.Write(buffer, size);

		if(WaitForSleep(10) < 0) {
			break;
		}
	}

	m_pcmfile.Close();

	free(buffer);
	buffer = NULL;

	return;
}

void CAudioManage::PrintHwParams(snd_pcm_hw_params_t *params)
{
	int dir;
	unsigned int val,val2;  
	snd_pcm_uframes_t frames;

	snd_pcm_hw_params_get_channels(params, &val);  
	printf("channels = %d\n", val);  
	snd_pcm_hw_params_get_rate(params, &val, &dir);  
	printf("rate = %d bps\n", val);  
	snd_pcm_hw_params_get_period_time(params, &val, &dir);  
	printf("period time = %d us\n", val);  
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);  
	printf("period size = %d frames\n", (int)frames);  
	snd_pcm_hw_params_get_buffer_time(params, &val, &dir);  
	printf("buffer time = %d us\n", val);  
	snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *) &val);  
	printf("buffer size = %d frames\n", val);  
	snd_pcm_hw_params_get_periods(params, &val, &dir);  
	printf("periods per buffer = %d frames\n", val);  
	snd_pcm_hw_params_get_rate_numden(params, &val, &val2);  
	printf("exact rate = %d/%d bps\n", val, val2);  
	val = snd_pcm_hw_params_get_sbits(params);  
	printf("significant bits = %d\n", val);  
	//snd_pcm_hw_params_get_tick_time(params,  &val, &dir);  
	printf("tick time = %d us\n", val);  
	val = snd_pcm_hw_params_is_batch(params);  
	printf("is batch = %d\n", val);  
	val = snd_pcm_hw_params_is_block_transfer(params);  
	printf("is block transfer = %d\n", val);  
	val = snd_pcm_hw_params_is_double(params);  
	printf("is double = %d\n", val);  
	val = snd_pcm_hw_params_is_half_duplex(params);  
	printf("is half duplex = %d\n", val);  
	val = snd_pcm_hw_params_is_joint_duplex(params);  
	printf("is joint duplex = %d\n", val);  
	val = snd_pcm_hw_params_can_overrange(params);  
	printf("can overrange = %d\n", val);  
	val = snd_pcm_hw_params_can_mmap_sample_resolution(params);  
	printf("can mmap = %d\n", val);
	val = snd_pcm_hw_params_can_pause(params);
	printf("can pause = %d\n", val);
	val = snd_pcm_hw_params_can_resume(params);
	printf("can resume = %d\n", val);
	val = snd_pcm_hw_params_can_sync_start(params);
	printf("can sync start = %d\n", val);

	return;
}


