/*
 * thread_exit_foo.c
 *
 *  Created on: 2015年8月14日
 *      Author: leon
 */

#include "apue.h"
#include <pthread.h>

struct foo {
	int a, b, c, d;
};

void printfoo(const char *s, const struct foo *fp) {
	printf("%s", s);
	printf("  structure at %p\n", fp);
	printf("  foo.a = %d\n", fp->a);
	printf("  foo.b = %d\n", fp->b);
	printf("  foo.c = %d\n", fp->c);
	printf("  foo.d = %d\n", fp->d);
}

void *thr_fn1(void *arg) {
//	struct foo	foo = {1, 2, 3, 4};
	struct foo	*fp;

	if((fp = malloc(sizeof(struct foo))) == NULL)
		err_sys("malloc error");

	fp->a = 1;
	fp->b = 2;
	fp->c = 3;
	fp->d = 4;

	printfoo("thread 1: \n", fp);
	pthread_exit((void *)fp);
}

void *thr_fn2(void *arg) {
	printf("thread 2: ID is %lu\n", pthread_self());
	pthread_exit((void *)0);
}

int main(void) {
	int			err;
	pthread_t	tid1, tid2;
	struct foo	*fp;

	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if(err != 0)
		err_quit("can't create thread 1: %s\n", strerror(err));

	/*
	 * fp points to foo struct in thread 1 stack,
	 * the foo is overwrited after thread 1 exits.
	 */
	err = pthread_join(tid1, (void *)&fp);
	if(err != 0)
		err_quit("can't join with thread 1: %s\n", strerror(err));

	sleep(1);
	printf("parent starting second thread\n");

	/* thread 2 overwrite the foo space in thread 1 */
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if(err != 0)
		err_quit("can't create thread 2: %s\n", strerror(err));

	sleep(1);
	printfoo("parent:\n", fp);

	/* fp is allocated in thread, remember to free */
	free(fp);

	return 0;
}
