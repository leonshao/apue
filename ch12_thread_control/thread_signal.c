/*
 * thread_signal.c
 *
 *  Created on: 2015年9月10日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

int				quitflag;
sigset_t		mask;

pthread_mutex_t	lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	cond = PTHREAD_COND_INITIALIZER;

void *thr_fn(void *arg) {
	int			err, signo;
	pthread_t	tid = pthread_self();

	for( ; ; ) {
		err = sigwait(&mask, &signo);
		if(err != 0)
			err_exit(err, "sigwait failed");

		switch(signo) {
		case SIGINT:
			printf("\nthread: %ld, sig interrupt\n", tid);
			break;

		case SIGQUIT:
			printf("\nthread: %ld, sig quit\n", tid);
			pthread_mutex_lock(&lock);
			quitflag = 1;
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&cond);
			return (void*)0;

		default:
			printf("unexpected signal %d\n", signo);
			exit(1);
		}
	}

	return (void *)0;
}

int main(void) {
	int			err;
	sigset_t	oldmask;	/* can be used to unblock sigs later */
	pthread_t	tid;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);

	/* only block sigs for the main thread */
	if((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
		err_exit(err, "SIG_BLOCK error");

	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if(err != 0)
		err_exit(err, "can't create thread");

	pthread_mutex_lock(&lock);
	while(quitflag == 0)
		pthread_cond_wait(&cond, &lock);
	pthread_mutex_unlock(&lock);

	/* SIGQUIT has been caught and it now blocked; do whatever */
	quitflag = 0;

	/* reset signal mask which unblocks SIGQUIT */
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");

	printf("main thread %ld quits.\n", pthread_self());

	return 0;
}
