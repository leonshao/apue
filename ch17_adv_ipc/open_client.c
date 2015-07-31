/*
 * open_client.c
 *
 *  Created on: 2015年7月29日
 *      Author: leon
 */

#include "apue.h"
#include <sys/uio.h>	/* struct iovec */

int csopen(char *, int);

int main(void) {
	comm_client(csopen);
	return 0;
}

int csopen(char *name, int oflag) {
	static int 		fd[2] = {-1, -1};
	pid_t			pid;

	if(fd[0] < 0) {
		if(s_pipe(fd) < 0)
			err_sys("s_pipe error");

		if((pid = fork()) < 0) {
			err_sys("fork error");
		} else if (pid == 0) {		/* child */
			close(fd[0]);

			if( fd[1] != STDIN_FILENO &&
				dup2(fd[1], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error to stdin");

			if( fd[1] != STDOUT_FILENO &&
				dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error to stdout");

			if(execl("./open_server", "open_server", (char *)0) < 0)
				err_sys("execl error");
		}
		close(fd[1]);	/* parent */
	}

	return cl_open(fd[0], name, oflag);
}


