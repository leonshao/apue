/*
 * thread_getenv_safe.c
 *
 *  Created on: 2015年9月8日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>
#include <linux/limits.h>

/*
 * all threads share the same key
 */
static pthread_key_t	key;
static pthread_once_t	init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t			env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void thread_init(void) {
	pthread_key_create(&key, free);
}

char *getenv(const char *name) {
	int		i, len;
	char	*envbuf;

	/*
	 * the key just init once
	 */
	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);	/* not recursive mutex */
	envbuf = pthread_getspecific(key);
	if(envbuf == NULL) {
		envbuf = malloc(ARG_MAX);
		if(envbuf == NULL) {
			pthread_mutex_unlock(&env_mutex);
			return NULL;
		}

		pthread_setspecific(key, envbuf);
		printf("thread %ld set envbuf for key %d\n", pthread_self(), key);
	}

	len = strlen(name);
	for(i = 0; environ[i] != NULL; ++i) {
		if( (strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')) {
			strncpy(envbuf, &environ[i][len + 1], ARG_MAX);
			pthread_mutex_unlock(&env_mutex);
			return envbuf;
		}
	}
	pthread_mutex_unlock(&env_mutex);

	return NULL;
}

char *pathnames[] = {"PATH", "HOME"};

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

	printf("support multi threads...\n");
	for(i = 0; i < THREADNUM; ++i) {
		pthread_create(&tid, NULL, thr_fn, (void *)i);
	}

	while(1)
		sleep(1);

	return 0;
}
