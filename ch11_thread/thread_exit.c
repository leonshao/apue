/*
 * thread_exit.c
 *
 *  Created on: 2015年8月12日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

void *thr_fn1(void *arg) {
	printf("thread 1 returning\n");
	return (void *)1;
}

void *thr_fn2(void *arg) {
	printf("thread 2 exiting\n");
	pthread_exit((void *)2);
}

int main(void) {
	int			err;
	pthread_t	tid1, tid2;
	void		*tret;

	/* create threads */
	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if(err != 0)
		err_quit("can't create thread 1: %s\n", strerror(err));
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if(err != 0)
		err_quit("can't create thread 2: %s\n", strerror(err));

	/* join threads */
	err = pthread_join(tid1, &tret);
	if(err != 0)
		err_quit("can't join with thread 1: %s\n", strerror(err));
	printf("thread 1 exit with code %ld\n", (long)tret);

	err = pthread_join(tid2, &tret);
	if(err != 0)
		err_quit("can't join with thread 2: %s\n", strerror(err));
	printf("thread 2 exit with code %ld\n", (long)tret);

	return 0;
}
