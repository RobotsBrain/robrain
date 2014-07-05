#include <sys/types.h>
#include <unistd.h>

#include "rb_log.h"

void log_output_test(char *file, log_level level)
{
	int i = 5;
	float f = 9.5;
	char *str = "test";

	if(file != NULL)
		rb_log_init(file, level);

	RB_ERROR("---------log level: %d-----------\n", level);
	RB_INFO("Log for Info i=%d f=%f str=%s \n", i, f, str);
	RB_TRACE("Log for Trace i=%d f=%f str=%s \n", i, f, str);
	RB_DEBUG("Log for Debug i=%d f=%f str=%s \n", i, f, str);
	RB_WARN("Log for Warn i=%d f=%f str=%s \n", i, f, str);
	RB_ERROR("Log for Error i=%d f=%f str=%s \n", i, f, str);

	if(file != NULL)
		rb_log_uninit();

	return;
}

int main(int argc, char **argv)
{
	log_output_test("test.log", RB_LOG_INFO);

	log_output_test("test.log", RB_LOG_TRACE);

	log_output_test("test.log", RB_LOG_DEBUG);

	log_output_test("test.log", RB_LOG_WARN);

	log_output_test("test.log", RB_LOG_ERROR);

	log_output_test(NULL, RB_LOG_MIN);

	return 0;
}



