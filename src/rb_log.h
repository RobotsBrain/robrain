#ifndef __RB_LOG_H__
#define __RB_LOG_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>

#define RB_LOG_BUFFER_LEN	256


typedef enum log_level_en {
	RB_LOG_MIN,
	RB_LOG_INFO,
	RB_LOG_TRACE,
	RB_LOG_DEBUG,
	RB_LOG_WARN,
	RB_LOG_ERROR,
	RB_LOG_MAX
}log_level;

#define RB_INFO(fmt, args...)	\
	do {	\
		log_level rb_log_level = RB_LOG_INFO;	\
		if(rb_log_check_level(rb_log_level) == 0)	\
			break;	\
		char rb_log_buf[RB_LOG_BUFFER_LEN] = {0};	\
		snprintf(rb_log_buf, RB_LOG_BUFFER_LEN, "Pid: %d, Name: %s, Func: %s, Line: %d", \
					getpid(), __FILE__, __FUNCTION__, __LINE__);	\
		rb_log_printf(rb_log_level, rb_log_buf, fmt, ##args);	\
	} while(0)

#define RB_TRACE(fmt, args...)	\
	do {	\
		log_level rb_log_level = RB_LOG_TRACE;	\
		if(rb_log_check_level(rb_log_level) == 0)	\
			break;	\
		char rb_log_buf[RB_LOG_BUFFER_LEN] = {0};	\
		snprintf(rb_log_buf, RB_LOG_BUFFER_LEN, "Pid: %d, Name: %s, Func: %s, Line: %d", \
					getpid(), __FILE__, __FUNCTION__, __LINE__);	\
		rb_log_printf(rb_log_level, rb_log_buf, fmt, ##args);	\
	} while(0)

#define RB_DEBUG(fmt, args...)	\
	do {	\
		log_level rb_log_level = RB_LOG_DEBUG;	\
		if(rb_log_check_level(rb_log_level) == 0)	\
			break;	\
		char rb_log_buf[RB_LOG_BUFFER_LEN] = {0};	\
		snprintf(rb_log_buf, RB_LOG_BUFFER_LEN, "Pid: %d, Name: %s, Func: %s, Line: %d", \
					getpid(), __FILE__, __FUNCTION__, __LINE__);	\
		rb_log_printf(rb_log_level, rb_log_buf, fmt, ##args);	\
	} while(0)

#define RB_WARN(fmt, args...)	\
	do {	\
		log_level rb_log_level = RB_LOG_WARN;	\
		if(rb_log_check_level(rb_log_level) == 0)	\
			break;	\
		char rb_log_buf[RB_LOG_BUFFER_LEN] = {0};	\
		snprintf(rb_log_buf, RB_LOG_BUFFER_LEN, "Pid: %d, Name: %s, Func: %s, Line: %d", \
					getpid(), __FILE__, __FUNCTION__, __LINE__);	\
		rb_log_printf(rb_log_level, rb_log_buf, fmt, ##args);	\
	} while(0)

#define RB_ERROR(fmt, args...)	\
	do {	\
		log_level rb_log_level = RB_LOG_ERROR;	\
		if(rb_log_check_level(rb_log_level) == 0)	\
			break;	\
		char rb_log_buf[RB_LOG_BUFFER_LEN] = {0};	\
		snprintf(rb_log_buf, RB_LOG_BUFFER_LEN, "Pid: %d, Name: %s, Func: %s, Line: %d", \
					getpid(), __FILE__, __FUNCTION__, __LINE__);	\
		rb_log_printf(rb_log_level, rb_log_buf, fmt, ##args);	\
	} while(0)

void rb_log_init(const char *log_file, log_level level);

void rb_log_uninit(void);

int rb_log_check_level(log_level level);

void rb_log_printf(log_level level, const char *prefix, const char *fmt, ...);

#ifdef  __cplusplus
}
#endif
#endif
