/*
 * open_server.c
 *
 *  Created on: 2015年7月29日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	int 	nread;
	char	buf[MAXLINE];

	for( ; ; ) {	/* read arg buffer from client, process request */
		if((nread = read(STDIN_FILENO, buf, MAXLINE)) < 0)
			err_sys("read error");
		else if(nread == 0)
			break;	/* client has closed the stream pipe */

		write(STDERR_FILENO, buf, strlen(buf));
		write(STDERR_FILENO, "\n", 2);
		request(buf, nread, STDOUT_FILENO);
	}

	return 0;
}
