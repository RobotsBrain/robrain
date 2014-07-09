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
	RB_LOG_DEBUG,
	RB_LOG_INFO,
	RB_LOG_NOTICE,
	RB_LOG_WARNING,
	RB_LOG_ERROR,
	RB_LOG_CRIT,
	RB_LOG_ALERT,
	RB_LOG_EMERG,
	RB_LOG_MAX
}log_level;


#define RB_LOG(level, fmt, args...)	\
	do {	\
		if(rb_log_check_level(level) == 0)	\
			break;	\
		char rb_log_buf[RB_LOG_BUFFER_LEN] = {0};	\
		snprintf(rb_log_buf, RB_LOG_BUFFER_LEN, "pid(%d) %s %s(%d)", \
					getpid(), __FILE__, __FUNCTION__, __LINE__);	\
		rb_log_printf(level, rb_log_buf, fmt, ##args);	\
	} while(0)

#define RB_DEBUG(fmt, args...)	RB_LOG(RB_LOG_DEBUG, fmt, ##args)

#define RB_INFO(fmt, args...)	RB_LOG(RB_LOG_INFO, fmt, ##args)

#define RB_NOTICE(fmt, args...)	RB_LOG(RB_LOG_NOTICE, fmt, ##args)
	
#define RB_WARNING(fmt, args...)	RB_LOG(RB_LOG_WARNING, fmt, ##args)

#define RB_ERROR(fmt, args...)	RB_LOG(RB_LOG_ERROR, fmt, ##args)

#define RB_CRIT(fmt, args...)	RB_LOG(RB_LOG_CRIT, fmt, ##args)

#define RB_ALERT(fmt, args...)	RB_LOG(RB_LOG_ALERT, fmt, ##args)

#define RB_EMERG(fmt, args...)	RB_LOG(RB_LOG_EMERG, fmt, ##args)


void rb_log_init(const char *log_file, log_level level);

void rb_log_uninit(void);

int rb_log_check_level(log_level level);

void rb_log_printf(log_level level, const char *prefix, const char *fmt, ...);

#ifdef  __cplusplus
}
#endif
#endif
