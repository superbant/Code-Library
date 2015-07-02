#include "vp_log.h"

SZLOG *g_zlog = NULL;

const char *zlog_priority[] =
{
	"emergencies",
	"alerts",
	"critical",
	"errors",
	"warnings",
	"notifications",
	"informational",
	"debugging",
	NULL,
};


static void vzlog (int priority, const char *format, va_list args);
static int zlog_set_file (char *filename);
static int zlog_reset_file ();
static int zlog_rotate ();

/* Open log stream */
void zlog_open (const char *progname, int flags,
		int syslog_flags, int syslog_facility, int level)
{
	if(g_zlog != NULL)
	{
		return;
	}
	g_zlog = (SZLOG *)malloc(sizeof (SZLOG));
	if(g_zlog == NULL)
	{
		return;
	}
	memset (g_zlog, 0, sizeof (SZLOG));

	g_zlog->ident = progname;
	g_zlog->flags |= flags;
	g_zlog->facility = syslog_facility;
	g_zlog->maskpri = level;

	openlog (progname, syslog_flags, syslog_facility);

	pthread_mutex_init(&g_zlog->m_lock,NULL);
	char log_name[PATH_MAX]={0};
	snprintf(log_name,sizeof(log_name),"%s.log",progname);
	if ( flags & ZLOG_FILE)
    {
	    zlog_set_file(log_name);
    }
	return;
}

void zlog_close ()
{
	closelog();
	if(g_zlog == NULL)
	{
		return;
	}
	if(g_zlog->fp !=NULL)
	{
		fclose (g_zlog->fp);
		g_zlog->fp = NULL;
	}
	zlog_reset_file();
	pthread_mutex_destroy(&g_zlog->m_lock);
	if(g_zlog != NULL)
	{
		free(g_zlog);
		g_zlog = NULL;
	}
}


void zlog (int priority, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vzlog (priority, format, args);
	va_end (args);
}

void zlog_err (const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vzlog (LOG_ERR, format, args);
	va_end (args);
}

void zlog_warn (const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vzlog (LOG_WARNING, format, args);
	va_end (args);
}


void zlog_info (const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vzlog (LOG_INFO, format, args);
	va_end (args);
}

void zlog_debug (const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vzlog (LOG_DEBUG, format, args);
	va_end (args);
}

/* Called from command.c. */
void zlog_set_flag (int flags)
{
	g_zlog->flags |= flags;
}

void zlog_reset_flag (int flags)
{
	g_zlog->flags &= ~flags;
}

static int zlog_set_file (char *filename)
{
	FILE *fp;
	/* Open file. */
	fp = fopen (filename, "a");
	if (fp == NULL)
		return 0;

	/* Set flags. */
	g_zlog->filename = strdup (filename);
	g_zlog->fp = fp;

	return 1;
}

/* Reset opend file. */
static int zlog_reset_file ()
{
	pthread_mutex_lock(&g_zlog->m_lock);
	g_zlog->flags &= ~ZLOG_FILE;

	if (g_zlog->fp)
	{
		fclose (g_zlog->fp);
		g_zlog->fp = NULL;
	}
	if (g_zlog->filename)
	{
		free (g_zlog->filename);
		g_zlog->filename = NULL;
	}
	pthread_mutex_unlock(&g_zlog->m_lock);
	return 1;
}

/* Reopen log file. */
static int zlog_rotate ()
{
	FILE *fp;

	if (g_zlog->filename == NULL)
	{
		return -1;
	}
	if (g_zlog->fp)
	{
		fclose (g_zlog->fp);
		g_zlog->fp = NULL;
	}

	char buf_time[TIME_BUF_LENGTH]={0};
	char buf_log_name[PATH_MAX]={0};
	get_time_str(buf_time,sizeof(buf_time),1);
	snprintf(buf_log_name,sizeof(buf_log_name),"%s.%s",g_zlog->filename, buf_time);
	rename(g_zlog->filename, buf_log_name);

	fp = fopen (g_zlog->filename, "a");
	if (fp == NULL)
	{
		return -1;
	}
	g_zlog->fp = fp;
	g_zlog->fsize = 0;
	return 1;
}

/* Utility routine for current time printing. */
char *get_time_str(char *buf, int len,int type)
{
	time_t t=time(NULL);
	struct tm *ptm;
	ptm = localtime(&t);
	if(type == 1)
	{
        strftime(buf,len,"%Y%m%d%H%M%S",ptm);
	}
	else
	{
        strftime(buf,len,"%Y-%m-%d %T",ptm);
	}
	return buf;

}

/* va_list version of z_log. */
static void vzlog (int priority, const char *format, va_list args)
{
	/* If z_log is not specified, use default one. */

	char buf_log[LOG_BUF_LENGTH]={0};
	char buf_time[TIME_BUF_LENGTH]={0};

	get_time_str(buf_time,sizeof(buf_time),0);

	int n =	snprintf(buf_log,sizeof(buf_log),"[level:%s time:%s pid:%u tid:%ld]",
	        zlog_priority[priority], buf_time,getpid(),pthread_self());
	vsnprintf(buf_log+n,sizeof(buf_log)-n,format,args);
	/* When g_zlog is NULL, use stderr for logging. */
	if (g_zlog == NULL)
	{
		fprintf (stderr, "%s",buf_log);
		fflush (stderr);
		/* In this case we return at here. */
		return;
	}
	/* only log this information if it has not been masked out */
	if ( priority  >  g_zlog->maskpri)
	{
		return ;
	}

	/* Syslog output */
	if (g_zlog->flags & ZLOG_SYSLOG)
	{
		syslog (priority|g_zlog->facility, "%s", buf_log);
	}
	/* File output. */
	if (g_zlog->flags & ZLOG_FILE)
	{
		pthread_mutex_lock(&g_zlog->m_lock);
		if(g_zlog->fsize > LOG_FILE_SIZE_MAX)
		{
			zlog_rotate();
		}
		pthread_mutex_unlock(&g_zlog->m_lock);
		g_zlog->fsize += fprintf (g_zlog->fp, "%s",buf_log);
		fflush (g_zlog->fp);
	}
	/* stdout output. */
	if (g_zlog->flags & ZLOG_STDOUT)
	{
		fprintf (stdout, "%s",buf_log);
		fflush (stdout);
	}

	/* stderr output. */
	if (g_zlog->flags & ZLOG_STDERR)
	{
		fprintf (stderr, "%s",buf_log);
		fflush (stderr);
	}
}

