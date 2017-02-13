/*
 * getc.c
 *
 *  Created on: 2017年2月13日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	int c;
	while((c = getc(stdin)) != EOF)
		if(putc(c, stdout) == EOF)
			err_sys("output error");

	if(ferror(stdin))
		err_sys("input error");

	return 0;
}
