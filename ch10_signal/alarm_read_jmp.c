/*
 * alarm_read_jmp.c
 *
 *  Created on: 2015年10月9日
 *      Author: leon
 */

#include "apue.h"
#include <setjmp.h>

static jmp_buf	env_alrm;

static void sig_alrm(int signo) {
	printf("catch SIGALRM\n");
	longjmp(env_alrm, 1);
}

int main(void) {
	int		n;
	char	line[MAXLINE];

	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	if(setjmp(env_alrm) != 0)
		err_quit("read timeout");

	alarm(2);
	if((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	alarm(0);

	write(STDOUT_FILENO, line, n);

	return 0;
}
