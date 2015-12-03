/*
 * echoall.c
 *
 *  Created on: 2015年12月3日
 *      Author: leon
 */

#include "apue.h"

int main(int argc, char *argv[]) {
	int				i;
	char			**ptr;
	extern char 	**environ;

	for(i = 0; i < argc; ++i)	/* echo all command-line args */
		printf("argv[%d]: %s\n", i, argv[i]);

	for(ptr = environ; *ptr != 0; ++ptr)
		printf("%s\n", *ptr);

	return 0;
}
