/*
 * sleep2_jmp.c
 *
 *  Created on: 2015年10月8日
 *      Author: leon
 */

#include "apue.h"
#include <setjmp.h>

static jmp_buf	env_alrm;

static void sig_alrm(int signo) {
	printf("catch SIGALRM\n");
	longjmp(env_alrm, 1);
}

unsigned int sleep2(unsigned int nsecs) {
	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		return nsecs;

	if(setjmp(env_alrm) == 0) {
		alarm(nsecs);	/* start the timer */
//		sleep(5);		/* debug, makes pause not called */
		printf("pause()\n");
		pause();		/* next caught signal wakes us up */
	}

	return alarm(0);	/* turn off timer, return unslept time */
}

static void sig_int(int signo) {
	int				i, j;
	volatile int	k;

	printf("\nsig_int starting\n");
	/*
	 * Tune these loops to run for more than 2 seconds
	 * on whatever system this test program is running.
	 */
	for(i = 0; i < 300000; i++)
		for(j = 0; j< 4000; j++)
			k += i * j;
	printf("sig_int finished\n");
}

int main(void) {
	unsigned int	unslept;

	if(signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");

	unslept = sleep2(2);
	printf("sleep2 returned: %u\n", unslept);

	return 0;
}
