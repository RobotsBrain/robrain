#include "Mp4Encoder.h"



int main(int argc, char **argv)
{
	Mp4Encoder encoder;

	encoder.WriteH264File("test.h264", "test.mp4");

	return 0;
}


// http://www.cnblogs.com/chutianyao/archive/2012/04/13/2446140.html