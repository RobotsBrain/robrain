#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>

#include "HlsServer.h"

using namespace std;



int main(int argc, char **argv)
{
	int c = -1, port = 0;
	int option_index = 0;
	string host, rootdir;
	static struct option long_options[] = {
		{"host", 1, 0, 0},
		{"port", 1, 0, 0},
		{"dir", 1, 0, 0},
		{"log", 1, 0, 0},
		{"daemon", 0, 0, 0},
		{0, 0, 0, 0}
	};

	while (1) {
		c = getopt_long(argc, argv, "H:P:D:L", long_options, &option_index);
		if (c == -1 || c == '?') {
			break;
		}

		if ((!c && !(strcasecmp(long_options[option_index].name, "host"))) || c == 'H') {
			host = optarg;
		} else if ((!c && !(strcasecmp(long_options[option_index].name, "port"))) || c == 'P') {
			port = atoi(optarg);
		} else if ((!c && !(strcasecmp(long_options[option_index].name, "dir"))) || c == 'D') {
			rootdir = optarg;
		} else {
			break;
		}
	}

	Hls::CHlsServer hls(host, port, rootdir);

	hls.Start();

	hls.Stop();
	
	return 0;
}


