/*
 * signal_usr.c
 *
 *  Created on: 2015年9月24日
 *      Author: leon
 */

#include "apue.h"

static void sig_usr(int); /* one handler for both signals */

int main(void) {
	if(signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("can't catch SIGUSR1");

	if(signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("can't catch SIGUSR2");

	for( ; ; )
		pause();

	return 0;
}

static void sig_usr(int signo) {
	if(signo == SIGUSR1)
		printf("received SIGUSR1\n");
	else if(signo == SIGUSR2)
		printf("received SIGUSR2\n");
	else
		err_dump("received signal %d\n", signo);
}
