/*
 * lock_test.c
 *
 *  Created on: 2014年12月10日
 *      Author: leon
 */

#include "apue.h"
#include <fcntl.h>

static void lockabyte(const char *name, int fd, off_t offset)
{
	if (writew_lock(fd, offset, SEEK_SET, 1) < 0)
		err_sys("%s: writew_lock error", name);
	printf("%s: got the lock, byte %ld\n", name, offset);
}

int main(void)
{
	int		fd;
	pid_t	pid;

	/*
	 * Create a file and write 2 bytes.
	 */
	if ((fd = creat("templock", FILE_MODE)) < 0)
		err_sys("creat error");
	if (write(fd, "ab", 2) < 0)
		err_sys("write error");

	/* TODO sync procs by TELL_WAIT */
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* child */
		lockabyte("child", fd, 0);
		/* TODO TELL_PARENT, WAIT_PARENT */
		/* use sleep ensure the first lockabyte is done */
		sleep(5);
		lockabyte("child", fd, 1);	/* always causes deadlock and returned */
	} else {					/* parent */
		lockabyte("parent", fd, 1);
		/* TODO TELL_CHILD, WAIT_CHILD */
		sleep(5);
		lockabyte("parent", fd, 0);
	}

	exit(0);
}
