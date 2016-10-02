#include <unistd.h>

#include "VideoCapture.h"



int main(int argc, char **argv)
{
	CVideoCapture capture;

	capture.Start();

	while(1) {
		sleep(5);
	}

	capture.Stop();

	return 0;
}


