/*
 * lock.c
 *
 *  Created on: 2014年12月10日
 *      Author: leon
 */

#include "apue.h"
#include <fcntl.h>

/*
 * Lock/unlock a file region
 */
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type		= type;		/* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_whence	= whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_start	= offset;	/* byte offset, relative to l_whence */
	lock.l_len		= len;		/* #bytes (0 means EOF) */

	return fcntl(fd, cmd, &lock);
}

/*
 * Test lock status
 */
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type		= type;		/* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_whence	= whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_start	= offset;	/* byte offset, relative to l_whence */
	lock.l_len		= len;		/* #bytes (0 means EOF) */

	if (fcntl(fd, F_GETLK, &lock) < 0)
		err_sys("fcntl error");

	if (lock.l_type == F_UNLCK)
		return 0;		/* false, region isn't locked by another proc */

	return lock.l_pid;	/* true, return pid of lock owner */
}

/*
 * Lock the whole opened file
 */
int lockfile(int fd)
{
	struct flock fl;

	fl.l_type	= F_WRLCK;		/* Write lock.	*/
	fl.l_start	= 0;			/* Begins.  */
	fl.l_whence	= SEEK_SET;		/* Seek from beginning of file.  */
	fl.l_len	= 0;			/* Lock until EOF */

	return fcntl(fd, F_SETLK, &fl);
}