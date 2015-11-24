/*
 * fork.c
 *
 *  Created on: 2015年11月24日
 *      Author: leon
 */

#include "apue.h"

int		glob = 6;		/* external variable in initialized data */
char 	buf[] = "a write to stdout\n";

int main(void) {
	int		var = 0;	/* automatic variable on the stack */
	pid_t	pid;

	/* buf is printed immediately, there is no buffer in write */
	if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
		err_sys("write error");

	printf("before fork\n");	/* we don't flush stdout */

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {		/* child */
		glob++;					/* modify variables */
		var++;
	} else {					/* parent */
		sleep(2);
	}

	printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	return 0;
}
