/*
 * not_reenter.c
 *
 *  Created on: 2015年9月24日
 *      Author: leon
 */

#include "apue.h"
#include <pwd.h>

static void my_alarm(int signo) {
	struct passwd		*rootptr;

	printf("in signal handler\n");
	/*
	 * in Linux, because the memory is locked by futex,
	 * the process is blocked here
	 */
	if((rootptr = getpwnam("syslog")) == NULL)
		err_sys("getpwnam(syslog) error");

	alarm(1);
}

int main(void) {
	struct passwd		*ptr;

	signal(SIGALRM, my_alarm);
	alarm(1);
	for( ; ; ) {
		if((ptr = getpwnam("leon")) == NULL)
			err_sys("getpwnam error");

		printf("get name %s\n", ptr->pw_name);

		if(strcmp(ptr->pw_name, "leon") != 0)
			printf("return value corrupted! pw_name = %s\n",
					ptr->pw_name);
	}
}
