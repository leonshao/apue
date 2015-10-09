/*
 * sleep1.c
 *
 *  Created on: 2015年10月8日
 *      Author: leon
 */

#include "apue.h"

static void sig_alrm(int signo) {
	/* nothing to do, just return to wake up the pause */
	printf("catch SIGALRM\n");
}

unsigned int sleep1(unsigned int nsecs) {
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		return nsecs;

	alarm(nsecs);		/* start the timer */
	pause();			/* next caught signal wakes us up */

	return alarm(0);	/* turn off timer, return unslept time */
}

int main(void) {
	sleep1(3);
	return 0;
}
