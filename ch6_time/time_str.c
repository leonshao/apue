/*
 * time_str.c
 *
 *  Created on: 2016年1月26日
 *      Author: leon
 */

#include "apue.h"
#include <time.h>

int main(void) {
	time_t		caltime;
	struct tm 	*time_ptr;
	char		buf[MAXLINE];

	if((caltime = time(NULL)) == -1)
		err_sys("time() error");

	if((time_ptr = localtime(&caltime)) == NULL)
		err_sys("localtime() error");

	if(strftime(buf, MAXLINE, "%F %T", time_ptr) == 0)
		err_sys("strftime() error");

	printf("%s\n", buf);
//	fputs(buf, stdout);

	return 0;
}
