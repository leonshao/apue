/*
 * system_ed.c
 *
 *  Created on: 2015年10月20日
 *      Author: leon
 */

#include "apue.h"

static void sig_int(int signo) {
	printf("%d: caught SIGINT\n", getpid());
}

static void sig_chld(int signo) {
	printf("%d: caught SIGCHLD\n", getpid());
}

int my_system(const char *cmdstring) {	/* with appropriate singla handling */
	struct sigaction	ignore, saveintr, savequit;
	sigset_t			chldmask, savemask;
	pid_t				pid;
	int					status;

	if(cmdstring == NULL)
		return 1;	/* always a command processor with UNIX */

	ignore.sa_handler = SIG_IGN;
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	if(sigaction(SIGINT, &ignore, &saveintr) < 0)
		return -1;
	if(sigaction(SIGQUIT, &ignore, &savequit) < 0)
		return -1;

	sigemptyset(&chldmask);		/* now block SIGCHLD */
	sigaddset(&chldmask, SIGCHLD);
	if(sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
		return -1;

	if((pid = fork()) < 0)
		status = -1;	/* probably out of processes */
	else if(pid == 0){			/* child */
		/* restore previous signal actions & reset signal mask */
		sigaction(SIGINT, &saveintr, NULL);
		sigaction(SIGQUIT, &savequit, NULL);
		sigprocmask(SIG_SETMASK, &savemask, NULL);

		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);	/* exec error */
	} else {					/* parent */
		printf("%d: parent waitpid\n", getpid());
		while(waitpid(pid, &status, 0) < 0)
			if(errno != EINTR) {
				status = -1;	/* error other than EINTR from waitpid() */
				break;
			}
	}

	printf("%d: parent restore signals & reset mask\n", getpid());
	/* restore previous signal actions & reset signal mask */
	if(sigaction(SIGINT, &saveintr, NULL) < 0)
		return -1;
	if(sigaction(SIGQUIT, &savequit, NULL) < 0)
		return -1;
	if(sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
		return -1;

	return status;
}

int main(void) {
	if(signal(SIGINT, sig_int) < 0)
		err_sys("signal(SIGINT) error");
	if(signal(SIGCHLD, sig_chld) < 0)
		err_sys("signal(SIGCHLD) error");

	/*
	 * When quit from /bin/ed, SIGCHLD is sent to
	 * this program(main).
	 * Thus, sig_chld is executed and "caught SIGCHLD"
	 * is printed out.
	 */
	if(my_system("/bin/ed") < 0)
		err_sys("system error");

	return 0;
}
