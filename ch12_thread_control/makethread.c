/*
 * makethread.c
 *
 *  Created on: 2015年9月6日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

int makethread(void *(fn)(void *), void *arg) {
	int				err;
	pthread_t		tid;
	pthread_attr_t	attr;

	err = pthread_attr_init(&attr);
	if(err != 0)
		return err;

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(err == 0)
		err = pthread_create(&tid, &attr, fn, arg);

	pthread_attr_destroy(&attr);

	return err;
}

void *thr_fn(void *arg) {
	pthread_t		tid = pthread_self();
	pthread_attr_t	attr;
	int 			err, detachstate;

	if(arg == NULL)
		printf("thread %ld starts...\n", tid);
	else
		printf("main thread %ld starts...\n", tid);

	err = pthread_attr_getdetachstate(&attr, &detachstate);
	if(err != 0)
		printf("thread %ld fails to get detach state.\n", tid);
	else
		printf("thread %ld gets detach state: %d\n", tid, detachstate);

	while(1)
		sleep(1);

	return (void *)0;
}

int main(void) {
	makethread(thr_fn, NULL);

	thr_fn((void *)1);

	return 0;
}
