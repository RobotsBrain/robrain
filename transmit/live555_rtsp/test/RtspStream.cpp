#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include "RtspStream.h"


#define FIFO_NAME       "/tmp/H264_fifo"
#define FILEBUFSIZE     (1024 * 1024)
#define BUFFERSIZE      PIPE_BUF 

CRTSPStream::CRTSPStream(void)
{
}

CRTSPStream::~CRTSPStream(void)
{
}

bool CRTSPStream::Init()
{
    if(access(FIFO_NAME, F_OK) == -1) {
        int res = mkfifo(FIFO_NAME, O_CREAT | O_EXCL | 0777);
        if(res != 0) {
            printf("Create fifo failed.\n");
            return false;
        }
    }

    m_pipe_fd = open(FIFO_NAME, O_WRONLY, 0);
    if(m_pipe_fd == -1) {
        printf("can not open pipe!\n");
        return 0;
    }

    return true;
}
  
void CRTSPStream::Uninit()
{
    close(m_pipe_fd);  
}  
  
bool CRTSPStream::SendH264File(const char *pFileName)  
{
    if(pFileName == NULL) {
        return false;
    }

    FILE *fp = fopen(pFileName, "rb");
    if(!fp) {
        printf("open file %s failed!", pFileName);
        return false;
    }

    fseek(fp, 0, SEEK_SET);
  
    unsigned char *buffer  = new unsigned char[FILEBUFSIZE];
    int pos = 0;

    while(1) {
        int readlen = fread(buffer + pos, sizeof(unsigned char), FILEBUFSIZE - pos, fp);
        if(readlen <= 0) {
            fseek(fp, 0, SEEK_SET);
            continue;  
        }
  
        readlen += pos;

        int writelen = SendH264Data(buffer, readlen);
        if(writelen <= 0) {
            printf("send error!\n");
            continue;
        }

        memcpy(buffer, buffer + writelen, readlen - writelen);
        pos = readlen - writelen;

        usleep(100);
    }
    
    fclose(fp);
    delete[] buffer;

    return true;
}  

int CRTSPStream::SendH264Data(const unsigned char *data, unsigned int size)
{
    int send_size = 0;
    int remain_size = size;

    while(send_size < size) {
        int data_len = (remain_size < BUFFERSIZE) ? remain_size : BUFFERSIZE;
        int len = write(m_pipe_fd, data + send_size, data_len);
        if(len == -1) {
            static int resend_conut = 0;

            if(errno == EAGAIN && ++resend_conut <= 3) {
                printf("write fifo error\n");
                continue;
            }

            resend_conut = 0;
            printf("write fifo error, errorcode[%d], send_size[%d]\n", errno, send_size);
            break;
        } else {
            send_size += len;
            remain_size -= len;
        }
    }

    return send_size;
}


