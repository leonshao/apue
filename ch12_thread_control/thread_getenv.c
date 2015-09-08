/*
 * thread_getenv.c
 *
 *  Created on: 2015年9月8日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>
#include <linux/limits.h>

static char envbuf[ARG_MAX];

extern char **environ;

/*
 * not support threads
 */
char *getenv(const char *name) {
	int		i, len;

	len = strlen(name);
	for(i = 0; environ[i] != NULL; ++i) {
		if( (strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')) {
			strncpy(envbuf, &environ[i][len + 1], sizeof(envbuf));
			return envbuf;
		}
	}

	return NULL;
}

pthread_mutex_t			env_mutex;
static pthread_once_t	init_done = PTHREAD_ONCE_INIT;

static void thread_init(void) {
	pthread_mutexattr_t		attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&env_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

/*
 * support for multi threads
 */
int getenv_r(const char *name, char *buf, int buflen) {
	int		i, len, olen;

	/*
	 * make sure function thread_init only executes one time
	 * by one thread
	 */
	pthread_once(&init_done, thread_init);
	len = strlen(name);
	pthread_mutex_lock(&env_mutex);
	for(i = 0; environ[i] != NULL; ++i) {
		if( (strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')) {
			olen = strlen(&environ[i][len + 1]);
			if(olen >= buflen) {
				pthread_mutex_unlock(&env_mutex);
				return ENOSPC;
			}
			strncpy(buf, &environ[i][len + 1], buflen);
			pthread_mutex_unlock(&env_mutex);
			return 0;
		}
	}

	pthread_mutex_unlock(&env_mutex);

	return ENOENT;
}

char *pathnames[] = {"PATH", "HOME"};

void *thr_fn_r(void *arg) {
	pthread_t	tid = pthread_self();
	long		i = (long)arg;
	char		*name = pathnames[i % 2];
	char 		envbuf[ARG_MAX];

	/*
	 * Thread privodes local buffer to get result.
	 * If use the global envbuf, it is still not safe,
	 * because the modification is out of control by
	 * this thread.
	 */
	if(getenv_r(name, envbuf, sizeof(envbuf)) == 0)
		printf("thread: %ld, i: %ld, pathname: %s, env: %s\n",
				tid, i, name, envbuf);
	else
		printf("can't not get env for thread: %ld, pathname: %s\n",
				tid, name);

	return (void *)0;
}

void *thr_fn(void *arg) {
	pthread_t	tid = pthread_self();
	long		i = (long)arg;
	char		*name = pathnames[i % 2];

	/*
	 * all threads use the same buffer, which is easy to
	 * be overwritten.
	 */
	printf("thread: %ld, i: %ld, pathname: %s, env: %s\n",
			tid, i, name, getenv(name));

	return (void *)0;
}

#define THREADNUM 	10

int main(void) {
	long		i;
	pthread_t	tid;

	printf("not support multi threads...\n");
	for(i = 0; i < THREADNUM; ++i) {
		pthread_create(&tid, NULL, thr_fn, (void *)i);
	}

	sleep(1);

	printf("support multi threads...\n");
	for(i = 0; i < THREADNUM; ++i) {
		pthread_create(&tid, NULL, thr_fn_r, (void *)i);
	}

	while(1)
		sleep(1);

	return 0;
}
