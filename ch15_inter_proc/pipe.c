/*
 * pipe.c
 *
 *  Created on: 2015年3月12日
 *      Author: leon
 */

#include "apue.h"

int main(void)
{
	int		fd[2];
	pid_t	pid;
	const char* pstr = "hello world\n";
	int 	n;
	char	line_read[MAXLINE] = {0};
	char	line[MAXLINE] = {0};

	if(pipe(fd) < 0)
		err_sys("pipe error");

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0)		/* parent */
	{
		close(fd[0]);
		printf("parent(%d) writes: %s", getpid(), pstr);
		write(fd[1], pstr, strlen(pstr));	/* write to child */
	}
	else					/* child */
	{
		close(fd[1]);
		read(fd[0], line_read, sizeof(line_read));
		n = snprintf(line, sizeof(line), "child(%d) reads: %s", getpid(), line_read);
		write(STDOUT_FILENO, line, n);		/* print */
	}

	return 0;
}
