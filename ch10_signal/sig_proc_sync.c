/*
 * sig_proc_sync.c
 *
 *  Created on: 2015年10月19日
 *      Author: leon
 */

#include "apue.h"

static volatile sig_atomic_t sigflag;	/* set nonzero by sig handler */
static sigset_t	newmask, oldmask, zeromask;

/*
 * one signal handler for SIGUSR1 and SIGUSR2
 */
static void sig_usr(int signo) {
	sigflag = 1;
}

void TELL_WAIT(void) {
	if(signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");
	if(signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR2) error");

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/*
	 * Block SIGUSR1 and SIGUSR2, and save current signal mask.
	 */
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
}

void TELL_PARENT(pid_t pid) {
	kill(pid, SIGUSR2);	/* tell parent we're done */
}

void WAIT_PARENT(void) {
	while(sigflag == 0)
		sigsuspend(&zeromask);	/* wait for parent */

	sigflag = 0;
	/*
	 * Reset signal mask to original value.
	 */
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

void TELL_CHILD(pid_t pid) {
	kill(pid, SIGUSR1);	/* tell child we're done */
}

void WAIT_CHILD(void) { /* same as WAIT_PARENT */
	while (sigflag == 0)
		sigsuspend(&zeromask); /* wait for child */

	sigflag = 0;
	/*
	 * Reset signal mask to original value.
	 */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

int main(void) {
	pid_t	pid, mypid;

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if(pid > 0) {	// parent
		mypid = getpid();
		TELL_WAIT();

		printf("%d: WAIT_CHILD()\n", mypid);
		WAIT_CHILD();
		printf("%d: wake up\n", mypid);

		sleep(2);
		printf("%d: TELL_CHILD()\n", mypid);
		TELL_CHILD(pid);
	} else {			// child
		mypid = getpid();
		TELL_WAIT();

		sleep(2);
		printf("%d: TELL_PARENT()\n", mypid);
		TELL_PARENT(getppid());

		printf("%d: WAIT_PARENT()\n", mypid);
		WAIT_PARENT();

		printf("%d: wake up\n", mypid);
	}

	return 0;
}
