/*
 * apue.h
 *
 *  Created on: 2014年12月8日
 *      Author: leon
 */

#ifndef INCLUDE_APUE_H_
#define INCLUDE_APUE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>	/* for waitpid */
#include <sys/stat.h>
#include <errno.h>

#define	MAXLINE 	4096	/* max line length */

#define FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Error parts
 */
void err_sys(const char *, ...);
void err_quit(const char *, ...);

/*
 * File I/O parts
 */
void set_fl(int fd, int flags);
void clr_fl(int fd, int flags);

ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);

/*
 * Lock parts
 */
/* Needs () for fd, offset, etc. Possibly expressions */
#define read_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))
#define is_read_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len);
int lockfile(int fd);

/*
 * Process sync parts
 */
void TELL_WAIT();

#endif /* INCLUDE_APUE_H_ */
