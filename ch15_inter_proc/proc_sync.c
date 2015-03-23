/*
 * proc_sync.c
 *
 *  Created on: 2015年3月17日
 *      Author: leon
 */

#include "apue.h"

static int	pfd1[2], pfd2[2];

void TELL_WAIT(void)
{
	if(pipe(pfd1) < 0 || pipe(pfd2) < 0)
		err_sys("pipe error");
}

/* called by parent */
void WAIT_CHILD(void)
{
	char c;

	if(read(pfd2[0], &c, 1) != 1)
		err_sys("read error");

	if(c != 'c')
		err_quit("WAIT_CHILD: incorrect data %c", c);
}

/*
 * 	+-------------------+	+---------------+
 * 	|	parent			|	|		child	|
 * 	|		pfd1[1]	-----------> pfd1[0]	|
 * 	|					|	|				|
 * 	|		pfd2[0] <----------- pfd2[1]	|
 * 	|					|	|				|
 * 	+-------------------+	+---------------+
 */

/* called by child to wake up parent */
void TELL_PARENT(pid_t pid)
{
	if(write(pfd2[1], "c", 1) != 1)
		err_sys("write error");
}

/* called by parent to wake up child */
void TELL_CHILD(pid_t pid)
{
	if(write(pfd1[1], "p", 1) != 1)
		err_sys("write error");
}

/* called by child */
void WAIT_PARENT(void)
{
	char c;

	/* read blocked by default */
	if(read(pfd1[0], &c, 1) != 1)
		err_sys("read error");

	/* remember to check if the byte read is correct */
	if(c != 'p')
		err_quit("WAIT_PARENT: incorrect data %c", c);
}

