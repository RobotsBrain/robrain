#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#include "rb_log.h"


size_t log_executable_path(char* processdir, char* processname, size_t len)
{
	char* path_end;

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

void log_output_test(char *file, log_level level)
{
	int i = 5;
	float f = 9.5;
	char *str = "test";

	if(file != NULL)
		rb_log_init(file, level);

	RB_EMERG("---------log level: %d-----------\n", level);

	RB_DEBUG("Log for Debug i=%d f=%f str=%s \n", i, f, str);
	RB_INFO("Log for Info i=%d f=%f str=%s \n", i, f, str);
	RB_NOTICE("Log for Notice i=%d f=%f str=%s \n", i, f, str);
	RB_WARNING("Log for Warning i=%d f=%f str=%s \n", i, f, str);
	RB_ERROR("Log for Error i=%d f=%f str=%s \n", i, f, str);
	RB_CRIT("Log for Crit i=%d f=%f str=%s \n", i, f, str);
	RB_ALERT("Log for Alert i=%d f=%f str=%s \n", i, f, str);
	RB_EMERG("Log for Emerg i=%d f=%f str=%s \n", i, f, str);

	if(file != NULL)
		rb_log_uninit();

	return;
}

int main(int argc, char **argv)
{
	log_level level;
	char path[PATH_MAX];
	char processname[128];

	log_executable_path(path, processname, sizeof(path));
	strcat(processname, ".log");

	for(level = RB_LOG_DEBUG; level < RB_LOG_MAX; level++)
		log_output_test(processname, level);

	log_output_test(NULL, RB_LOG_MIN);

	return 0;
}



