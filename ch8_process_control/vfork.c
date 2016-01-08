/*
 * vfork.c
 *
 *  Created on: 2015年11月24日
 *      Author: leon
 */

#include "apue.h"

int		glob = 6;		/* external variable in initialized data */

int main(void) {
	int		var, n;		/* automatic variable on the stack */
	pid_t	pid;

	var = 88;
	printf("before vfork\n");
	if ((pid = vfork()) < 0)
		err_sys("vfork error");
	else if (pid == 0) {	/* child */
		glob++;				/* modify parent's variables */
		var++;
//		fflush(stdout);
//		close(STDOUT_FILENO);
		_exit(0);			/* child terminates */
//		exit(0);
	}

	sleep(1);
//	fflush(stdout);
//	close(STDOUT_FILENO);
	/*
	 * Parent continues here
	 */
	n = printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	printf("n = %d\n", n);
	return 0;
}
