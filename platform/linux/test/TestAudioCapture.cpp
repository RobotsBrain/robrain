#include <unistd.h>

#include "AudioManage.h"



int main(int argc, char **argv)
{
	CAudioManage audio;

	audio.Start();

	while(1) {
		sleep(10);
	}

	audio.Stop();

	return 0;
}


