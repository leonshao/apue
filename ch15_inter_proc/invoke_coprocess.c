/*
 * invoke_coprocess.c
 *
 *  Created on: 2015年3月29日
 *      Author: leon
 */

/*
 * 	+-------------------+			+---------------+
 * 	|	parent			|			|		child	|
 * 	|					|	pipe1	|				|
 * 	|		fd1[1]	------------------> stdin		|
 * 	|					|	pipe2	|				|
 * 	|		fd2[0]  <------------------ stdout		|
 * 	|					|			|				|
 * 	+-------------------+			+---------------+
 *
 *	parent fds
 *	==========
 *	COMMAND    PID USER   FD   TYPE DEVICE SIZE/OFF   NODE NAME
 *	invoke_co 4237 leon    0u   CHR 136,12      0t0     15 /dev/pts/12
 *	invoke_co 4237 leon    1u   CHR 136,12      0t0     15 /dev/pts/12
 *	invoke_co 4237 leon    2u   CHR 136,12      0t0     15 /dev/pts/12
 *	invoke_co 4237 leon    4w  FIFO    0,8      0t0 404122 pipe
 *	invoke_co 4237 leon    5r  FIFO    0,8      0t0 404123 pipe
 *
 *	child fds
 *	=========
 * 	COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF   NODE NAME
 * 	add2    4238 leon    0r  FIFO    0,8      0t0 404122 pipe
 * 	add2    4238 leon    1w  FIFO    0,8      0t0 404123 pipe
 * 	add2    4238 leon    2u   CHR 136,12      0t0     15 /dev/pts/12
 */

#include "apue.h"

/*
 * singnal handler for child exits
 * parent and child are connected by pipes,
 * once child exists before parent and parent
 * tries to write pipe1, SIGPIPE occurs
 */
static void sig_pipe(int);

int main(void)
{
	int		fd1[2], fd2[2], n;
	pid_t	pid;
	char	line[MAXLINE];

	if(signal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("signal error");

	if(pipe(fd1) < 0 || pipe(fd2) < 0)
		err_sys("pipe error");

	if((pid = fork()) <0)
		err_sys("fork error");
	else if(pid > 0)			/* parent */
	{
		close(fd1[0]);
		close(fd2[1]);

		while(fgets(line, MAXLINE, stdin) != NULL)
		{
			/*
			 * get input and write n bytes to child by fd1
			 * no need to write the whole line of MAXLINE bytes
			 */
			n = strlen(line);
			if(write(fd1[1], line, n) != n)
				err_sys("write error to pipe1");

			if((n = read(fd2[0], line, MAXLINE)) < 0)
				err_sys("read error from pipe2");

			if(n == 0)
			{
				err_msg("child closed pipe2");
				break;
			}

			line[n] = 0;	/* null terminate */
			if(fputs(line, stdout) == EOF)
				err_sys("fputs error");
		}

		if(ferror(stdin))
			err_sys("fgets error on stdin");

		return 0;
	}
	else						/* child */
	{
		close(fd1[1]);
		close(fd2[0]);

		if(fd1[0] != STDIN_FILENO)
		{
			if(dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error to stdin");
			close(fd1[0]);
		}

		if(fd2[1] != STDOUT_FILENO)
		{
			if(dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error to stdout");
			close(fd2[1]);
		}

		if(execl("./add2", "add2", (char *)0) < 0)
			err_sys("execl error");
	}

	return 0;
}

static void sig_pipe(int signo)
{
	printf("SIGPIPE caught\n");
	exit(1);
}
