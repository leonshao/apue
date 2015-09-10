/*
 * thread_putenv.c
 *
 *  Created on: 2015年9月10日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>
#include <linux/limits.h>

static pthread_key_t	key;
static pthread_once_t	init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t			lock = PTHREAD_MUTEX_INITIALIZER;

static void thread_init(void) {
	pthread_key_create(&key, free);
}

int putenv_r(char *env) {
	int			err, envlen, ret = -1;
	char 		*envbuf;
	sigset_t	mask, oldmask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	/* blocks signals to become signal safe */
	if((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
		err_exit(err, "SIG_BLOCK error");

	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&lock);	/* blocks other threads */
	envbuf = pthread_getspecific(key);
	if(envbuf == NULL) {
		/* thread private space to copy name & value */
		envbuf = malloc(ARG_MAX);
		if(envbuf == NULL) {
			pthread_mutex_unlock(&lock);
			return ret;
		}

		pthread_setspecific(key, envbuf);
	}

	envlen = strlen(env);
	strncpy(envbuf, env, envlen + 1);
	ret = putenv(envbuf);

	pthread_mutex_unlock(&lock);

	/* reset to not block the signals */
	if((err = pthread_sigmask(SIG_SETMASK, &oldmask, NULL)) != 0)
		err_exit(err, "SIG_SETMASK error");

	return ret;
}

#define THREADNUM 	10

int main(void) {
	char 	buf[128];

	return 0;
}
