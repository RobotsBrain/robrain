#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifdef __linux__
	#include <sys/prctl.h>
#endif

#include "base/Log.h"
#include "base/ThreadLoop.h"



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
#ifdef __linux__
		char name[20] = {0};

		prctl(PR_GET_NAME, name);
		INFO("thread name(%s)\n", name);
#endif

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


