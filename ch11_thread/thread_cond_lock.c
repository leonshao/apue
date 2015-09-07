/*
 * thread_cond_lock.c
 *
 *  Created on: 2015年8月26日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

struct msg {
	struct msg *m_next;
	int			id;
	/* more stuff */
};

struct msg *workq;
pthread_cond_t	qready 	= PTHREAD_COND_INITIALIZER;
pthread_mutex_t	qlock 	= PTHREAD_MUTEX_INITIALIZER;

void process_msg(void) {
	pthread_t	tid = pthread_self();
	struct msg *mp;

	for( ; ; ) {
		pthread_mutex_lock(&qlock);
		while(workq == NULL) {
			pthread_cond_wait(&qready, &qlock);
			printf("thread %ld wakes up\n", tid);
		}

		/* get and remove message from workq */
		mp = workq;
		workq = workq->m_next;
		pthread_mutex_unlock(&qlock);

		/* now process the message mp */
		printf("thread %ld process msg %d\n", tid, mp->id);
	}
}

void enqueue_msg(struct msg *mp) {
	pthread_mutex_lock(&qlock);
	mp->m_next = workq;
	workq = mp;
	printf("msg %d enqueued\n", mp->id);
	pthread_mutex_unlock(&qlock);
//	pthread_cond_signal(&qready);
//	pthread_cond_broadcast(&qready);
}

#define NMSGS		20		/* total messages */
#define NTHREADS	3		/* total threads */

void *thr_fn(void *arg) {
	process_msg();

	return (void *)0;
}

int main(void) {
	int			i, err;
	pthread_t	tid;
	struct msg *mp;

	for (i = 0; i < NTHREADS; ++i) {
		err = pthread_create(&tid, NULL, thr_fn, NULL);
		if (err != 0)
			err_quit("can't create thread: %s\n", strerror(err));

		printf("new thread %ld\n", tid);
	}

	for (i = 0; i < NMSGS; ++i) {
		if((mp = malloc(sizeof(struct msg))) != NULL) {
			mp->m_next = NULL;
			mp->id = i;

			enqueue_msg(mp);

		}
	}
//	pthread_cond_signal(&qready);
	pthread_cond_broadcast(&qready);

//	while(workq != NULL)
	while(1)
		sleep(1);

	return 0;
}
