/*
 * open_client.c
 *
 *  Created on: 2015年7月29日
 *      Author: leon
 */

#include "apue.h"
#include <sys/uio.h>	/* struct iovec */

#define CL_OPEN	"open"	/* client's request for server */

int csopen(char *, int);

#define BUFFSIZE	8192

int main(int argc, char *argv[]) {
	char	buf[BUFFSIZE], line[MAXLINE];
	int		fd, len, n;

	/* read filename to cat from stdin */
	while(fgets(line, MAXLINE, stdin) != NULL) {
		len = strlen(line);
		if(line[len - 1] == '\n')
			line[len - 1] = 0;	/* replace newline with null */

		/* open file */
		if((fd = csopen(line, O_RDONLY)) < 0)
			continue;	/* csopen prints error from server */

		/* and cat to stdout */
		while((n = read(fd, buf, BUFFSIZE)) > 0)
			if(write(STDOUT_FILENO, buf, n) != n)
				err_sys("write error");

		if(n < 0)
			err_sys("read error");
	}

	return 0;
}

int csopen(char *name, int oflag) {
	static int 		fd[2] = {-1, -1};
	pid_t			pid;
	char			buf[10];
	struct iovec	iov[3];
	int				len;

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

	snprintf(buf, sizeof(buf), " %d", oflag);	/* oflag to ascii */
	iov[0].iov_base = CL_OPEN " ";			/* string concatenation */
	iov[0].iov_len	= strlen(CL_OPEN) + 1;
	iov[1].iov_base	= name;
	iov[1].iov_len	= strlen(name);
	iov[2].iov_base = buf;
	iov[2].iov_len	= strlen(buf) + 1;		/* +1 for null at end of buf */
	len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;
	if(writev(fd[0], &iov[0], 3) != len)
		err_sys("writev error");

	/* read descriptor, returned errors handled by write() */
	return recv_fd(fd[0], write);
}


