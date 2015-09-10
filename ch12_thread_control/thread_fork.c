/*
 * pthread_fork.c
 *
 *  Created on: 2015年9月10日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

pthread_mutex_t	lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void) {
	printf("preparing locks...\n");
	pthread_mutex_lock(&lock1);
	pthread_mutex_lock(&lock2);
}

void parent(void) {
	printf("parent unlocking locks...\n");
	pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void child(void) {
	printf("child unlocking locks...\n");
	pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void *thr_fn(void *arg) {
	printf("thread %ld started...\n", pthread_self());
	pause();

	return (void *)0;
}

int main(void) {
	int			err;
	pthread_t	tid;
	pid_t		pid;

#if defined(BSD) || defined(MACOS)
	printf("pthread_atfork is unsupported\n");
#else
	if((err = pthread_atfork(prepare, parent, child)) != 0)
		err_exit(err, "can't install fork handlers");

	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if(err != 0)
		err_exit(err, "can't create thread");

	sleep(2);
	printf("parent about to fork...\n");
	if((pid = fork()) < 0)
		err_quit("for failed");
	else if(pid == 0)	/* child */
		printf("child returned from fork\n");
	else				/* parent */
		printf("parent returned from fork\n");
#endif

	return 0;
}
