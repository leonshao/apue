/*
 * system.c
 *
 *  Created on: 2016年1月4日
 *      Author: leon
 */

#include "apue.h"
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

int my_system(const char *cmdstring) {	/* version without signal handling */
	pid_t	pid;
	int		status;

	if(cmdstring == NULL)
		return 1;	/* always a command processor with UNIX */

	if((pid = fork()) < 0)
		status = -1;
	else if(pid == 0) {
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);	/* execl error */
	}
	else {
		while(waitpid(pid, &status, 0) < 0) {
			if(errno != EINTR) {
				status = -1;	/* error other than EINTR from waitpid() */
				break;
			}
		}
	}

	return status;
}

int main(void) {
	int		status;

	if((status = my_system("date")) < 0)
		err_sys("system() error");
	pr_exit(status);
	printf("\n");

	if((status = my_system("nosuchcommand")) < 0)
		err_sys("system() error");
	pr_exit(status);
	printf("\n");

	if((status = my_system("who; exit 44")) < 0)
		err_sys("system() error");
	pr_exit(status);

	return 0;
}
