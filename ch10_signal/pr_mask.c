/*
 * pr_mask.c
 *
 *  Created on: 2015年10月9日
 *      Author: leon
 */

#include "apue.h"

void pr_mask(const char *str) {
	sigset_t	sigset;
	int			errno_save;

	errno_save = errno;		/* we can be called by signal handlers */
	if(sigprocmask(0, NULL, &sigset) < 0)
		err_sys("sigprocmask error");

	printf("%s", str);

	if(sigismember(&sigset, SIGINT)) printf("SIGINT ");
	if(sigismember(&sigset, SIGQUIT)) printf("SIGQUIT ");
	if(sigismember(&sigset, SIGUSR1)) printf("SIGUSR1 ");
	if(sigismember(&sigset, SIGALRM)) printf("SIGALRM ");

	/* remaining signals can go here */

	printf("\n");

	errno = errno_save;
}

int main(void) {
	sigset_t	newmask;

	pr_mask("sig init mask: ");
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);
	if(sigprocmask(SIG_BLOCK, &newmask, NULL) < 0)
		err_sys("SIG_BLOCK error");

	pr_mask("sig mask after SIG_BLOCK: ");
	return 0;
}
