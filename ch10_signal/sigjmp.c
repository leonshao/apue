/*
 * sigjmp.c
 *
 *  Created on: 2015年10月15日
 *      Author: leon
 */

#include "apue.h"
#include <setjmp.h>
#include <time.h>

static void 					sig_usr1(int), sig_alrm(int);
static sigjmp_buf				jmpbuf;
static volatile sig_atomic_t	canjmp;

int main(void) {
	if(signal(SIGUSR1, sig_usr1) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");

	if(signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	pr_mask("starting main: ");

	if(sigsetjmp(jmpbuf, 1)) {
//	if(setjmp(jmpbuf)) {	/* main sig mask is not saved */
		pr_mask("ending main: ");
		return 0;
	}

	canjmp = 1;

	for( ; ; )
		pause();

	return 0;
}

static void sig_usr1(int signo) {
	time_t	starttime;

	if(canjmp == 0)
		return;		/* unexpected signal, ignore */

	pr_mask("starting sig_usr1: ");
	alarm(3);				/* SIGALRM in 3 seconds */
	starttime = time(NULL);
	for( ; ; )				/* busy wait for 5 seconds */
		if(time(NULL) > starttime + 5)
			break;
	pr_mask("finishing sig_usr1: ");

	canjmp = 0;
	siglongjmp(jmpbuf, 1);	/* jump back to main, don't return */
//	_longjmp(jmpbuf, 1);
}

static void sig_alrm(int signo) {
	pr_mask("in sig_alrm: ");
}
