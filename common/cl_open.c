/*
 * cl_open.c
 *
 *  Created on: 2015年7月31日
 *      Author: leon
 */

#include "apue.h"

void comm_client(int (*csopen)(char *name, int oflag)) {
	char	buf[BUFFSIZE], line[MAXLINE];
	int		fd, len, n;

	/* read filename to cat from stdin */
	while(fgets(line, MAXLINE, stdin) != NULL) {
		len = strlen(line);
		if(line[len - 1] == '\n')
			line[len - 1] = 0;	/* replace newline with null */

		/* open file */
		if((fd = (*csopen)(line, O_RDONLY)) < 0)
			continue;	/* csopen prints error from server */

		/* and cat to stdout */
		while((n = read(fd, buf, BUFFSIZE)) > 0)
			if(write(STDOUT_FILENO, buf, n) != n)
				err_sys("write error");

		if(n < 0)
			err_sys("read error");
	}
}

/*
 * sending the name and oflag to the server
 */
int cl_open(int fd, char *name, int oflag) {
	char			buf[10];
	struct iovec	iov[3];
	int				len;

	snprintf(buf, sizeof(buf), " %d", oflag);	/* oflag to ascii */
	iov[0].iov_base = CL_OPEN " ";			/* string concatenation */
	iov[0].iov_len	= strlen(CL_OPEN) + 1;
	iov[1].iov_base	= name;
	iov[1].iov_len	= strlen(name);
	iov[2].iov_base = buf;
	iov[2].iov_len	= strlen(buf) + 1;		/* +1 for null at end of buf */
	len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;
	if(writev(fd, &iov[0], 3) != len)
		err_sys("writev error");

	/* read descriptor, returned errors handled by write() */
	return recv_fd(fd, write);
}
