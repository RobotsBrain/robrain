#include <string.h>
#include <stdio.h>
#include <string>

#include "base/MsgQueue.h"



int main(int argc, char **argv)
{
	int i = 0;
	Base::CMsgQueue queue("test_queque");
	std::string msg[] = {"test 1", "test 2", "test 3"};

	printf("queue name: %s\n", queue.GetName());

	for(i = 0; i < 3; i++) {
		queue.SendMsg(i + 1, (void *)msg[i].c_str());
	}

	unsigned int type;
	void *data = NULL;

	for(i = 0; i < 3; i++) {
		queue.RecvMsg(type, &data);
		printf("type: %d, msg: %s\n", type, (char *)data);
	}

	return 0;
}


