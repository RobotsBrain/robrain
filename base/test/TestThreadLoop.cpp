#include <sys/prctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "Log.h"
#include "ThreadLoop.h"



class TestThreadLoop : public Base::CThreadLoop
{
public:
	TestThreadLoop();
	~TestThreadLoop();

	bool Start();
	bool Stop();
	
	virtual void EventHandleLoop();
};

TestThreadLoop::TestThreadLoop()
: Base::CThreadLoop("TestThreadLoop")
{
}

TestThreadLoop::~TestThreadLoop()
{
}

bool TestThreadLoop::Start()
{
	DEBUG("\n");

	return StartThread();
}

bool TestThreadLoop::Stop()
{
	DEBUG("\n");

	return StopThread();
}

void TestThreadLoop::EventHandleLoop()
{
	while(1) {
		char name[20] = {0};

		prctl(PR_GET_NAME, name);
		INFO("thread name(%s)\n", name);

		if(WaitForSleep(10000) < 0) {
			break;
		}
	}

	return;
}


/*****************************************************************************************/

int main(int argc, char **argv)
{
	TestThreadLoop test;

	test.Start();

	for(int i = 0; i < 6; i++) {
		sleep(5);
	}

	test.Stop();

	return 0;
}


