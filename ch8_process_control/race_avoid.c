/*
 * race_avoid.c
 *
 *  Created on: 2016年1月8日
 *      Author: leon
 */

#include "apue.h"

static void charactatime(char *);

int main(void) {
	pid_t	pid;

	TELL_WAIT();

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		WAIT_PARENT();		/* parent goes first */
		charactatime("output from child\n");
		TELL_PARENT(getppid());
	}
	else {
		charactatime("output from parent\n");
		TELL_CHILD(pid);
		WAIT_CHILD();
	}

	return 0;
}

static void charactatime(char *str) {
	char	*ptr;
	int		c;

	setbuf(stdout, NULL);	/* set unbuffered */
	for(ptr = str; (c = *ptr++) != 0;)
		putc(c, stdout);
}
