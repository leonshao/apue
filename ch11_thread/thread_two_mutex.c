/*
 * thread_two_mutex.c
 *
 *  Created on: 2015年8月20日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

#define NHASH		29
#define	HASH(fp)	(((unsigned long)fp) % NHASH)
struct foo {
	int				f_count;
	pthread_mutex_t	f_lock;
	struct foo 		*f_next;	/* protected by hashlock */
	int				f_id;
	/* more stuff... */
};
struct foo *fh[NHASH];	/* array of lists */

pthread_mutex_t		hashlock = PTHREAD_MUTEX_INITIALIZER;

void fh_init() {
	int 	i;

	pthread_mutex_lock(&hashlock);
	for(i = 0; i < NHASH; ++i)
		fh[i] = NULL;

	pthread_mutex_unlock(&hashlock);
}

/*
 * allocate the object
 */
struct foo *foo_alloc(int id) {
	struct foo *fp;
	int			idx;

	if((fp = malloc(sizeof(struct foo))) != NULL) {
		fp->f_count = 0;

		if(pthread_mutex_init(&fp->f_lock, NULL) != 0) {
			free(fp);
			return NULL;
		}

		idx = HASH(fp);
		pthread_mutex_lock(&hashlock);
		fp->f_next = fh[idx];
		fh[idx] = fp;	/* front insert */

		/*
		 * If there are other fields of fp to initialize,
		 * lock f_lock before unlock hashlock.
		 * Once unlock hashlock first, it is possible that
		 * the object is locked/get/found by other thread before
		 * initialization completes.
		 * The hashlock is able to prevent the object from being
		 * touched by other threads.
		 */
		pthread_mutex_lock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
		/* continue initialization */
		fp->f_id = id;
		pthread_mutex_unlock(&fp->f_lock);
		printf("object id %d at hash index %d\n", id, idx);
	}

	return fp;
}

/*
 * add a reference to the object
 */
void foo_hold(struct foo *fp) {
	pthread_t	tid = pthread_self();
	if(fp == NULL) {
		err_msg("get NULL fp in foo_hold\n");
		return;
	}

	pthread_mutex_lock(&fp->f_lock);
	++fp->f_count;
	printf("foo_hold: thread %ld gets lock, object id %d, f_count %d\n",
					tid, fp->f_id, fp->f_count);
	pthread_mutex_unlock(&fp->f_lock);
}

/*
 * find an existing object
 */
struct foo *foo_find(struct foo *ifp, int id) {
	struct foo *fp;
	int			idx;

	idx = HASH(ifp);

	pthread_mutex_lock(&hashlock);
	for(fp = fh[idx]; fp != NULL; fp = fp->f_next) {
		if(fp->f_id == id) {
			foo_hold(fp);
			break;		/* if return here, unlock then return */
		}
	}
	pthread_mutex_unlock(&hashlock);
	return fp;
}

/*
 * release a reference to the object
 * return 0 for not removing the object,
 * return 1 for removing the object
 */
int foo_rele(struct foo *fp) {
	struct foo *tfp;
	int			idx;
	pthread_t	tid = pthread_self();

	pthread_mutex_lock(&fp->f_lock);
	if(fp->f_count == 1) {	/* last reference */
		/*
		 * unlock fp->f_lock before lock hashlock, can't
		 * lock hashlock directly here, otherwise,
		 * it causes dead lock
		 */
		pthread_mutex_unlock(&fp->f_lock);

		pthread_mutex_lock(&hashlock);
		pthread_mutex_lock(&fp->f_lock);
		/* need to recheck the condition */
		if(fp->f_count != 1) {
			fp->f_count--;
			printf("foo_rele: thread %ld gets lock, object id %d, f_count %d\n",
							tid, fp->f_id, fp->f_count);
			pthread_mutex_unlock(&fp->f_lock);
			pthread_mutex_unlock(&hashlock);
			return 0;
		}

		/* remove from the list */
		idx = HASH(fp);
		tfp = fh[idx];
		if(tfp == fp) {	/* fp is the first element */
			fh[idx] = fp->f_next;
		} else {
			while(tfp->f_next != fp)	/* traverse the list */
				tfp = tfp->f_next;
			tfp->f_next = fp->f_next;
		}

		/*
		 * unlock hashlock first to have other threads
		 * get the fh as soon as possible.
		 * fp is to free so unlock f_lock later.
		 */
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_unlock(&fp->f_lock);
		pthread_mutex_destroy(&fp->f_lock);
		printf("foo_rele: thread %ld free object id %d\n",
				tid, fp->f_id);
		free(fp);
		return 1;
	} else {
		fp->f_count--;
		printf("foo_rele: thread %ld gets lock, object id %d, f_count %d\n",
						tid, fp->f_id, fp->f_count);
		pthread_mutex_unlock(&fp->f_lock);
	}

	return 0;
}


#define	NOBJS	100
#define THREADNUM 	1000

pthread_t	thdarr[THREADNUM];

void *thr_fn(void *arg) {
	int			id, i;
	pthread_t	tid = pthread_self();
	struct foo *fp = NULL;

	id = tid % NOBJS;

	for(i = 0; i < NHASH; ++i) {
		if(fh[i] != NULL) {
			fp = foo_find(fh[i], id);
			if(fp != NULL){	/* object found */
				printf("thread %ld finds object id %d at hash idx %d\n",
						tid, id, i);
				break;
			}
		}
	}

	if(fp != NULL) {	/* object already hold in foo_find */
		sleep(1);
		foo_rele(fp);
	} else {
		printf("thread %ld can't find id %d\n", tid, id);
	}

	printf("thread %ld returns\n", tid);
	return (void *)0;
}

int main(void) {
	struct foo *fp;
	int			i, err, thrjoin;
	pthread_t	tid;

	fh_init();

	for (i = 0; i < NOBJS; ++i) {
		if ((fp = foo_alloc(i)) == NULL)
			err_sys("foo_alloc error");

	}

	for (i = 0; i < THREADNUM; ++i) {
		err = pthread_create(&tid, NULL, thr_fn, NULL);

		if (err != 0)
			err_quit("can't create thread: %s\n", strerror(err));

		thdarr[i] = tid;
		printf("new thread %ld\n", tid);
	}

	while(1) {
		thrjoin = 0;
		for(i = 0; i < THREADNUM; ++i) {
			if(thdarr[i] != 0) {
				pthread_join(thdarr[i], NULL);
				thdarr[i] = 0;
				thrjoin = 1;
			}
		}

		if(thrjoin == 0)
			return 0;
		sleep(1);
	}
}
