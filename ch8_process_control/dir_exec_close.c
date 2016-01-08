/*
 * dir_exec_close.c
 *
 *  Created on: 2016年1月9日
 *      Author: leon
 */

#include "apue.h"
#include <dirent.h>

#define	ROOTDIR		"/"

static void check_fd_cloexec(int fd);

int main(void) {
	DIR		*dp;
	int		fd;

	/*
	 * The fd status is set to default value FD_CLOEXEC,
	 * if dir is opened by opendir(). When exec is called,
	 * the opened dir is auto closed.
	 */
	dp = opendir(ROOTDIR);
	fd = dirfd(dp);
	check_fd_cloexec(fd);

	/*
	 * For the fd get by open(), the fd status is NOT set
	 * to FD_CLOEXEC, which remains open when exec is called.
	 */
	fd = open(ROOTDIR, O_RDONLY, S_IROTH);
	check_fd_cloexec(fd);

	return 0;
}

static void check_fd_cloexec(int fd) {
	int		val;
	val = fcntl(fd, F_GETFD, 0);

	switch (val) {
	case FD_CLOEXEC:
		printf("FD_CLOEXEC\n");
		break;

	default:
		printf("not FD_CLOEXEC\n");
	}
}
