/*
 * print_fd_fl.c
 *
 *  Created on: 2017年2月25日
 *      Author: leon
 */

#include "apue.h"

int main(int argc, char *argv[]) {
	int val, fd;

	if(argc != 2)
		err_quit("usage: a.out <descriptor#>");

	fd = atoi(argv[1]);
	if((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl error for fd %d", fd);

	switch(val & O_ACCMODE) {
	case O_RDONLY:
		printf("read only");
		break;

	case O_WRONLY:
		printf("write only");
		break;

	case O_RDWR:
		printf("read write");
		break;

	default:
		err_dump("unknown access mode");
	}

	if(val & O_APPEND)
		printf(", append");

	if(val & O_NONBLOCK)
		printf(", nonblocking");

#if defined(O_SYNC)
	if(val & O_SYNC)
		printf(", synchronous writes");
#endif
#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC)
	if(val & O_FSYNC)
		printf(", synchronous writes");
#endif
	putchar('\n');

	return 0;
}
