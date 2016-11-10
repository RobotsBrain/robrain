#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>



int GetOneNalu(char *pBufIn, int nInSize, char *pNalu, int &nNaluSize)
{
	char *p = pBufIn;
	int nStartPos = 0, nEndPos = 0;

	while (1) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
			nStartPos = p - pBufIn;
			break;
		}

		p++;

		if (p - pBufIn >= nInSize - 4) {
			return 0;
		}
	}

	p++;

	while (1) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
			nEndPos = p - pBufIn;
			break;
		}

		p++;

		if (p - pBufIn >= nInSize - 4) {
			nEndPos = nInSize;
			break;
		}
	}

	nNaluSize = nEndPos - nStartPos;
	memcpy(pNalu, pBufIn + nStartPos, nNaluSize);

	return 1;
}

int main(int argc, char **argv)
{
	char *buf = NULL;
	int fd, n;
	struct stat fstat;
	char out[8192];

    stat(argv[1], &fstat);

	fd = open(argv[1], O_RDONLY|O_NONBLOCK);
	if(fd < 0) {
		exit(1);
	}

	buf = (char *)malloc(fstat.st_size);

	n = read(fd, buf, fstat.st_size);

	int nNaluSize = 0;
	int offset = 0;

	while(offset < fstat.st_size) {
		if (GetOneNalu(buf + offset, fstat.st_size - offset, out, nNaluSize) == 0) {
			return -1;
		}

		int type = out[4] & 0x1f;

		printf("%d\n", type);

		offset += nNaluSize;
	}
	
	free(buf);

	close(fd);

	return 0;
}


