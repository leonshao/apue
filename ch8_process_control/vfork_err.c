/*
 * vfork_err.c
 *
 *  Created on: 2016年1月8日
 *      Author: leon
 */

#include "apue.h"

static void f1(void), f2(void);

int main(void) {
	/*
	 * Child returns from f1 and invoke f2, which breaks the stack.
	 * Later, when parent also returns from f1, for the stack is
	 * already overwritten by child, parent gets the invalid
	 * return address. Core dumped.
	 */
	f1();
	f2();
	return 0;
}

static void f1(void) {
	pid_t	pid;

	if((pid = vfork()) < 0)
		err_sys("vfork error");

	/* child and parent both return */
}

static void f2(void) {
	char	buf[1000];
	int		i;

	for(i = 0; i < 1000; ++i)
		buf[i] = 0;

	i = buf[0];
}

