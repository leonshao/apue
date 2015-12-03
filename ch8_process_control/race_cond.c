/*
 * race_cond.c
 *
 *  Created on: 2015年12月3日
 *      Author: leon
 */

#include "apue.h"

static void charactatime(char *);

int main(void) {
	pid_t	pid;

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)
		charactatime("output from child\n");
	else
		charactatime("output from parent\n");

	return 0;
}

static void charactatime(char *str) {
	char	*ptr;
	int		c;

	setbuf(stdout, NULL);	/* set unbuffered */
	for(ptr = str; (c = *ptr++) != 0;)
		putc(c, stdout);
}
