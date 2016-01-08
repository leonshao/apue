/*
 * defunct.c
 *
 *  Created on: 2016年1月9日
 *      Author: leon
 */

#include "apue.h"

#define PSCMD	"ps -o pid,ppid,state,tty,command"

int main(void) {
	pid_t	pid;

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid == 0)	/* child */
		exit(0);

	sleep(1);
	system(PSCMD);

	return 0;
}
/*
 * output as below:
PID  PPID S TT       COMMAND
3693  2605 S pts/11   bash
13729  3693 S pts/11   ./defunct
13730 13729 Z pts/11   [defunct] <defunct>
13731 13729 S pts/11   sh -c ps -o pid,ppid,state,tty,command
13732 13731 R pts/11   ps -o pid,ppid,state,tty,command
 */
