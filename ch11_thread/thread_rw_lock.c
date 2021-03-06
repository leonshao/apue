/*
 * thread_rw_lock.c
 *
 *  Created on: 2015年8月25日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

struct job {
	struct job	*j_next;
	struct job	*j_prev;
	pthread_t	j_id;	/* tells which thread handles the job */
	/* more stuff */
};

struct queue {
	struct job			*q_head;
	struct job			*q_tail;
	pthread_rwlock_t	q_lock;

	pthread_cond_t		q_cond_lock;
	pthread_mutex_t		q_mutex_lock;
};

/*
 * Initialize a queue
 */
int queue_init(struct queue *qp) {
	int	err;

	qp->q_head = NULL;
	qp->q_tail = NULL;

	err = pthread_rwlock_init(&qp->q_lock, NULL);
	if (err != 0)
		return err;

	err = pthread_cond_init(&qp->q_cond_lock, NULL);
	if (err != 0)
		return err;

	err = pthread_mutex_init(&qp->q_mutex_lock, NULL);
	if (err != 0)
		return err;

	/* continue initialization */

	return 0;
}

/*
 * Returns if the queue is empty
 */
int queue_is_empty(struct queue *qp) {
	return qp->q_head == NULL;
}

/*
 * Insert a job at the head of the queue.
 */
void job_insert(struct queue *qp, struct job *jp) {
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = qp->q_head;
	jp->j_prev = NULL;

	if(qp->q_head != NULL) {
		qp->q_head->j_prev = jp;
	} else
		qp->q_tail = jp;	/* empty queue */

	qp->q_head = jp;
	pthread_rwlock_unlock(&qp->q_lock);

	/*
	 * wake up all threads
	 */
	pthread_cond_broadcast(&qp->q_cond_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void job_append(struct queue *qp, struct job *jp) {
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = NULL;
	jp->j_prev = qp->q_tail;

	if(qp->q_tail != NULL) {
		qp->q_tail->j_next = jp;
	} else
		qp->q_head = jp;	/* empty queue */

	qp->q_tail = jp;
	pthread_rwlock_unlock(&qp->q_lock);
	pthread_cond_broadcast(&qp->q_cond_lock);
}

/*
 * Remove the given job from a queue.
 */
void job_remove(struct queue *qp, struct job *jp) {
	pthread_rwlock_wrlock(&qp->q_lock);
	if(jp == qp->q_head) {
		/* remove from head */
		qp->q_head = jp->j_next;
		if(qp->q_tail == jp)
			qp->q_tail = NULL;	/* queue becomes empty */
		else
			jp->j_next->j_prev = NULL;
	} else if(jp == qp->q_tail) {
		/* remove from tail */
		qp->q_tail = jp->j_prev;
		if(qp->q_head == jp)
			qp->q_head = NULL;	/* queue becomes empty */
		else
			jp->j_prev->j_next = NULL;
	} else {
		jp->j_prev->j_next = jp->j_next;
		jp->j_next->j_prev = jp->j_prev;
	}

	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *job_find(struct queue *qp, pthread_t id) {
	struct job 	*jp;

	/* return value checking is required */
	if(pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return NULL;

	/*
	 * this can be modified by checking head and tail
	 * first for efficiency
	 */
	for(jp = qp->q_head; jp != NULL; jp = jp->j_next)
		if(jp->j_id == id)
			break;

	pthread_rwlock_unlock(&qp->q_lock);

	return jp;
}

#define	NJOBS		20		/* total jobs */
#define NTHREADS	100		/* total threads */

struct queue	gq;			/* global queue object */
pthread_t		thdarr[NTHREADS];

/*
 * consumer
 */
void *thr_fn(void *arg) {
	struct job 	*jp;
	pthread_t	tid = pthread_self();

	for ( ; ; ) {
		/*
		 * Add condition lock instead of sleep.
		 * However, even one job inserted, producer wakes up
		 * all threads. When the queue is not NULL, everyone
		 * try to find its job although there is only one job
		 * in the queue. It waits time and resource for most
		 * threads on the procedure calls and "can't find job".
		 */
		pthread_mutex_lock(&gq.q_mutex_lock);
		while(gq.q_head == NULL) {
			pthread_cond_wait(&gq.q_cond_lock, &gq.q_mutex_lock);
			printf("thread %ld wakes up\n", tid);
		}
		pthread_mutex_unlock(&gq.q_mutex_lock);

		if((jp = job_find(&gq, tid)) != NULL) {
			job_remove(&gq, jp);
			printf("thread %ld gets job\n", tid);
			free(jp);
		} else
			printf("thread %ld can't find job\n", tid);
	}

	return (void *)0;
}

int main(void) {
	int 		i, err, idx;
	pthread_t 	tid;
	struct job 	*jp;

	queue_init(&gq);

	for (i = 0; i < NTHREADS; ++i) {
		err = pthread_create(&tid, NULL, thr_fn, NULL);
		if (err != 0)
			err_quit("can't create thread: %s\n", strerror(err));

		thdarr[i] = tid;
		printf("new thread %ld\n", tid);
	}

	sleep(1);
	/*
	 * producer
	 */
	for (i = 0; i < NJOBS; ++i) {
		if((jp = malloc(sizeof(struct job))) != NULL) {
			jp->j_next = NULL;
			jp->j_prev = NULL;
			idx = i % NTHREADS;	/* get thread tid */
			jp->j_id = thdarr[idx];

			job_insert(&gq, jp);

			printf("job id %ld created\n", jp->j_id);
		}
	}

	while(!queue_is_empty(&gq))
		sleep(1);

	return 0;
}
