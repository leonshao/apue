/*
 * alarm_read.c
 *
 *  Created on: 2015年10月9日
 *      Author: leon
 */

#include "apue.h"
#include <setjmp.h>

static void sig_alrm(int);

int main(void){
	int		n;
	char	line[MAXLINE];

	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	alarm(3);
	if((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	alarm(0);

	write(STDOUT_FILENO, line, n);
	return 0;
}

static void sig_alrm(int signo) {
	/* nothing to do, just return to interrupt the read */
	printf("catch SIGALRM\n");
}
