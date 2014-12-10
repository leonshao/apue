/*
 * err.c
 *
 *  Created on: 2014年12月8日
 *      Author: leon
 */

#include "apue.h"
#include <errno.h>	// for errno
#include <stdarg.h>	// for va_start

static void err_doit(int, int, const char *, va_list);

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag", which decides if to
 * print the error message of errno
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];
	size_t len;

	vsnprintf(buf, MAXLINE, fmt, ap);
	len = strlen(buf);

	if(errnoflag)
		snprintf(buf + len, MAXLINE - len, ": %s", strerror(error));

	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all the stdio output streams */
}
