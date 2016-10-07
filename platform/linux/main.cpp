#include <unistd.h>

#include "VideoCapture.h"
#include "AudioManage.h"
#include "AvFrameRingBuffer.h"
#include "AvStreamDump.h"



int main(int argc, char **argv)
{
	CVideoCapture capture;
	CAudioManage audio;	
	CAvStreamDump dump;

	capture.Start();
	audio.Start();
	dump.Start();

	while(1) {
		sleep(5);
	}

	dump.Stop();
	audio.Stop();
	capture.Stop();

	return 0;
}


