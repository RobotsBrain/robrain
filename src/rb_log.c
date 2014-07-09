#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "rb_log.h"


	
char *log_info[] = {
	[RB_LOG_MIN] = "",
	[RB_LOG_DEBUG] = "Debug",
	[RB_LOG_INFO] = "Info",
	[RB_LOG_NOTICE] = "Notice",
	[RB_LOG_DEBUG] = "Debug",
	[RB_LOG_WARNING] = "Warning",
	[RB_LOG_ERROR] = "Error",
	[RB_LOG_CRIT] = "Crit",
	[RB_LOG_ALERT] = "Alert",
	[RB_LOG_EMERG] = "Emerg",
	[RB_LOG_MAX] = "",
};

typedef struct rb_log_st {
	FILE *file;
	log_level level;
}rb_log;

rb_log * g_rb_log = NULL;

void rb_log_init(const char *log_file, log_level level)
{
	 if(g_rb_log != NULL)
		return;

	g_rb_log = (rb_log *)malloc(sizeof(rb_log));
	if(g_rb_log == NULL)
		return;

	memset(g_rb_log, 0, sizeof(rb_log));

	if(log_file == NULL)
		g_rb_log->file = stderr;
	else {
		g_rb_log->file = fopen(log_file, "a+");
		if(g_rb_log->file == NULL)
			g_rb_log->file = stderr;	
	}

	if(level <= RB_LOG_MIN || level >= RB_LOG_MAX )
		g_rb_log->level = RB_LOG_ERROR;
	else
		g_rb_log->level = level;

	return;
}

void rb_log_uninit(void)
{
	if(g_rb_log == NULL)
		return;

	fclose(g_rb_log->file);
	g_rb_log->file = NULL;
	free(g_rb_log);
	g_rb_log = NULL;

	return;
}

int rb_log_check_level(log_level level)
{
	if(g_rb_log == NULL)
		return 1;

	return (g_rb_log->level > level ? 0:1);
}

void rb_log_printf(log_level level, const char *prefix, const char *fmt, ...)
{
	FILE *fp = NULL;
	va_list args;
	time_t t_secs;
	struct tm *t_info;

	if(g_rb_log == NULL)
		fp = stderr;
	else
		fp = g_rb_log->file;

	time(&t_secs);
	t_info = gmtime(&t_secs);

	fprintf(fp, "%d-%d-%d %d:%d:%d %s %s: ", t_info->tm_year + 1900, t_info->tm_mon + 1, 
				t_info->tm_mday, t_info->tm_hour, t_info->tm_min, t_info->tm_sec, prefix, log_info[level]);

	va_start(args, fmt);
	vfprintf(fp, fmt, args);
	va_end(args);

	return;
}



