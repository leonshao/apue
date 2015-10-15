/*
 * pending.c
 *
 *  Created on: 2015年10月13日
 *      Author: leon
 */

#include "apue.h"

static void sig_quit(int);

int main(void) {
	sigset_t	newmask, oldmask, pendmask;

	if(signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("can't catch SIGQUIT");

	/*
	 * Block SIGQUIT and save current signal mask.
	 */
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	sleep(5);	/* SIGQUIT here will remain pending */

	if(sigpending(&pendmask) < 0)
		err_sys("sigpending error");
	if(sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	/*
	 * Reset signal mask which unblocks SIGQUIT.
	 * If there is pending SIGQUIT signal,
	 * sig_quit is executed before sigprocmask returns.
	 */
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
	printf("SIGQUIT unblocked\n");

	sleep(5);	/* SIGQUIT here will terminate with core file */
	return 0;
}

static void sig_quit(int signo) {
	printf("caught SIGQUIT\n");
	if(signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
}
