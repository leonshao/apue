/*
 * fgets.c
 *
 *  Created on: 2017年2月15日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	char buf[MAXLINE];

	while(fgets(buf, MAXLINE, stdin) != NULL)
		if(fputs(buf, stdout) == EOF)
			err_sys("output error");

	if(ferror(stdin))
		err_sys("input error");

	return 0;
}
