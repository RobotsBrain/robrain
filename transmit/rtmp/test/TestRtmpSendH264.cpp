#include <stdio.h>

#include "RtmpSendH264.h"



FILE *fp_send = NULL;

int read_buffer(unsigned char *buf, int buf_size)
{
	if(feof(fp_send)) {
        fseek(fp_send, 0L, SEEK_SET);
    }

	int true_size = fread(buf, 1, buf_size, fp_send);

	return true_size;
}

int main(int argc, char* argv[])
{
	fp_send = fopen("test.h264", "rb");

	//初始化并连接到服务器
	RTMP264_Connect("rtmp://localhost:1935/live/livestream");

	//发送
	RTMP264_Send(read_buffer);

	//断开连接并释放相关资源
	RTMP264_Close();

	return 0;
}


