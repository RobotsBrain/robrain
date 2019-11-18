#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#include "base/Log.h"


size_t log_executable_path(char* processdir, char* processname, size_t len)
{
	char* path_end = NULL;

	if(readlink("/proc/self/exe", processdir, len) <=0)
		return -1;

	path_end = strrchr(processdir, '/');
	if(path_end == NULL)
		return -1;

	++path_end;
	strcpy(processname, path_end);
	*path_end = '\0';

	return (size_t)(path_end - processdir);
}

void log_output_test(const char *file, Base::LOGLEVEL level)
{
	int i = 5;
	float f = 9.5;
	char str[] = "test";

	if(file != NULL) {
		Base::LogInit(file, level);
	}

	EMERG("---------log level: %d-----------\n", level);

	DEBUG("Log for Debug i=%d f=%f str=%s \n", i, f, str);
	INFO("Log for Info i=%d f=%f str=%s \n", i, f, str);
	NOTICE("Log for Notice i=%d f=%f str=%s \n", i, f, str);
	WARN("Log for Warning i=%d f=%f str=%s \n", i, f, str);
	ERROR("Log for Error i=%d f=%f str=%s \n", i, f, str);
	CRIT("Log for Crit i=%d f=%f str=%s \n", i, f, str);
	ALERT("Log for Alert i=%d f=%f str=%s \n", i, f, str);
	EMERG("Log for Emerg i=%d f=%f str=%s \n", i, f, str);

	if(file != NULL) {
		Base::LogUninit();
	}

	return;
}

int main(int argc, char **argv)
{
	int level;
	char path[PATH_MAX];
	char processname[128];

	log_executable_path(path, processname, sizeof(path));
	strcat(processname, ".log");

	for(level = Base::LOG_DEBUG; level <= Base::LOG_EMERG; level++) {
		log_output_test((const char *)processname, (Base::LOGLEVEL)level);
	}

	log_output_test(NULL, (Base::LOGLEVEL)-1);

	return 0;
}


