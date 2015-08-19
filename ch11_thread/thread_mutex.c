/*
 * thread_mutex.c
 *
 *  Created on: 2015年8月18日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

struct foo {
	int				f_count;
	pthread_mutex_t	f_lock;
	/* more stuff... */
};

#define THREADNUM 	5
struct foo *fptr = NULL;	/* global object pointer */

/*
 * allocate the object
 */
struct foo *foo_alloc(void) {
	struct foo *fp;

	if((fp = malloc(sizeof(struct foo))) == NULL)
		return fp;

	fp->f_count = 0;

	if(pthread_mutex_init(&fp->f_lock, NULL) != 0) {
		free(fp);
		return NULL;
	}

	return fp;
}

/*
 * add a reference to the object
 */
void foo_hold(struct foo *fp) {
	pthread_t tid = pthread_self();

	pthread_mutex_lock(&fp->f_lock);

	if(fp != NULL) {
		fp->f_count++;
		printf("foo_hold: thread %ld gets lock, f_count %d\n",
				tid, fp->f_count);
	} else {
		printf("foo_hold: fp is NULL in thread %ld\n", tid);
	}

	pthread_mutex_unlock(&fp->f_lock);
}

/*
 * release a reference to the object
 * any thread is able to destroy the lock and free the object
 */
void foo_rele(struct foo *fp) {
	pthread_t	tid = pthread_self();

	pthread_mutex_lock(&fp->f_lock);

	if(fp != NULL && fp->f_count > 0) {
		--fp->f_count;
		printf("foo_rele: thread %ld gets lock, f_count %d\n",
				tid, fp->f_count);

		if(fp->f_count == 0) {
			/* last thread */
			printf("destroy pthread mutex and object by thread %ld\n",
					tid);

			/* pay attention to unlock before destroy */
			pthread_mutex_unlock(&fp->f_lock);
			pthread_mutex_destroy(&fp->f_lock);
			free(fptr);
			fptr = NULL;	/* important to reset the global ptrs */

			return;
		}
	}

	pthread_mutex_unlock(&fp->f_lock);
}

void *thr_fn(void *arg) {
	int i;

	/* do limit job */
	for(i = 0; i < 2 && fptr != NULL; ++i) {
		foo_hold(fptr);
		sleep(1);
		foo_rele(fptr);	/* thread completes and releases object before exit */
	}

	printf("thread %ld returns\n", pthread_self());
	return (void *)NULL;
}

int main(void) {
	int			err, i;
	pthread_t	tid;

	if((fptr = foo_alloc()) == NULL)
		err_sys("foo_alloc error");

	for (i = 0; i < THREADNUM; ++i) {
		err = pthread_create(&tid, NULL, thr_fn, NULL);

		if (err != 0)
			err_quit("can't create thread: %s\n", strerror(err));

		printf("new thread %ld\n", tid);
	}

	tid = pthread_self();
	printf("main thread %ld\n", tid);

	/* makes the main thread equal to others */
	thr_fn(NULL);

	while(fptr != NULL)
		sleep(1);

	return 0;
}
