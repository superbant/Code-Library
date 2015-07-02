#ifndef _LOG_H_
#define _LOG_H_

#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

/* GCC have printf type attribute check.  */
#ifdef __GNUC__
#define PRINTF_ATTRIBUTE(a,b) __attribute__ ((__format__ (__printf__, a, b)))
#else
#define PRINTF_ATTRIBUTE(a,b)
#endif /* __GNUC__ */

#define ZLOG_NOLOG              0x00
#define ZLOG_FILE		0x01
#define ZLOG_SYSLOG		0x02
#define ZLOG_STDOUT             0x04
#define ZLOG_STDERR             0x08

#define ZLOG_NOLOG_INDEX        0
#define ZLOG_FILE_INDEX         1
#define ZLOG_SYSLOG_INDEX       2
#define ZLOG_STDOUT_INDEX       3
#define ZLOG_STDERR_INDEX       4
#define ZLOG_MAX_INDEX          5

/* For time string format. */
#define TIME_BUF_LENGTH 32 
#define LOG_BUF_LENGTH 10240
#define LOG_FILE_SIZE_MAX 5*1024

char *get_time_str(char *buf, int len, int type);

void zlog (int priority, const char *format, ...) PRINTF_ATTRIBUTE(2, 3);
void zlog_err (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
void zlog_warn (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
void zlog_info (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);
void zlog_debug (const char *format, ...) PRINTF_ATTRIBUTE(1, 2);

#define log_open(name) \
    openlog(name, LOG_CONS|LOG_PID|LOG_PERROR, LOG_USER);

#define ZLOG_OPEN(name) \
    zlog_open(name, ZLOG_FILE|ZLOG_SYSLOG, LOG_CONS|LOG_PID, LOG_USER, LOG_DEBUG);

#define log_error(log_fmt, log_arg...) \
    do{ \
	    char buf_time[TIME_BUF_LENGTH]={0};\
        syslog(LOG_ERR, "[time:%s file:%s line:%d function:%s]"log_fmt, \
        get_time_str(buf_time, TIME_BUF_LENGTH, 0), __FILE__, __LINE__, __FUNCTION__,##log_arg); \
    } while (0) 

#define log_warn(log_fmt, log_arg...) \
    do{ \
	    char buf_time[TIME_BUF_LENGTH]={0};\
        syslog(LOG_WARNING, "[time:%s file:%s line:%d function:%s]"log_fmt, \
        get_time_str(buf_time, TIME_BUF_LENGTH, 0), __FILE__, __LINE__, __FUNCTION__,##log_arg); \
    } while (0) 

#define log_info(log_fmt, log_arg...) \
    do{ \
	    char buf_time[TIME_BUF_LENGTH]={0};\
        syslog(LOG_INFO, "[time:%s file:%s line:%d function:%s]"log_fmt, \
        get_time_str(buf_time, TIME_BUF_LENGTH, 0), __FILE__, __LINE__, __FUNCTION__,##log_arg); \
    } while (0) 

#define ZLOG( log_level, fmt,  arg  ... ) \
	do{ \
	    char buf_time[TIME_BUF_LENGTH]={0};\
	    zlog( log_level, " [ file:%s line: %d function:%s ]" fmt ,\
		__FILE__, __LINE__, __FUNCTION__, ##arg );\
	} while(0)

#define ZLOGE( fmt,  arg  ... ) \
	do{ time_t tm = time(NULL); \
	    char buf_time[TIME_BUF_LENGTH]={0};\
	    zlog_err(" [ file:%s line:%d funtion:%s ]" fmt ,\
		__FILE__, __LINE__, __FUNCTION__, ##arg );\
	} while(0)

#define ZLOGW( fmt,  arg  ... ) \
	do{ \
	    char buf_time[TIME_BUF_LENGTH]={0};\
	    zlog_warn(" [ file:%s line: %d function:%s ]" fmt ,\
		__FILE__, __LINE__, __FUNCTION__, ##arg ); \
    } while(0)

#define ZLOGI( fmt,  arg  ... ) \
	do{ time_t tm = time(NULL); \
	    char buf_time[TIME_BUF_LENGTH]={0};\
	    zlog_info(" [ file:%s line: %d fuction:%s ]" fmt ,\
	    __FILE__, __LINE__, __FUNCTION__, ##arg );\
	} while(0)


#ifdef DEBUG  
#define log_debug(log_fmt, log_arg...) \
    do{ \
	    char buf_time[TIME_BUF_LENGTH]={0};\
        syslog(LOG_DEBUG, "[time:%s file%s line:%d function%s]"log_fmt, \
        get_time_str(buf_time, TIME_BUF_LENGTH, 0), __FILE__, __LINE__, __FUNCTION__,##log_arg); \
    } while (0) 

#define ZLOGD(fmt,  arg...) \
	do{ time_t tm = time(NULL); \
	    char buf_time[TIME_BUF_LENGTH]={0};\
	    zlog_debug(" [ file:%s line:%d funtion:%s ]" fmt ,\
		__FILE__, __LINE__, __FUNCTION__, ##arg );\
	} while(0)

#else
#define log_debug(log_fmt, log_arg...)
#define ZLOGD(fmt, arg ...)
#endif

typedef struct _z_log_
{
  const char *ident;
  int flags;
  FILE *fp;
  char *filename;
  int fsize;
  int maskpri;		/* as per syslog setlogmask */
  int priority;		/* as per syslog priority */
  int facility;		/* as per syslog facility */
  pthread_mutex_t m_lock;
}SZLOG;

/* Open z_log function */
void zlog_open (const char *, int, int, int, int);

/* Close z_log function. */
void zlog_close ();

/* Set z_log flags. */
void zlog_set_flag (int flags);
void zlog_reset_flag (int flags);
#endif
