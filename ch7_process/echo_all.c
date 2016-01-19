/*
 * echo_all.c
 *
 *  Created on: 2016年1月19日
 *      Author: leon
 */

#include "apue.h"

int main(int argc, char *argv[]) {
	int		i;

//	for(i = 0; i < argc; ++i)	/* echo all command-line args */
	for(i = 0; argv[i] != NULL; ++i)
		printf("argv[%d]: %s\n", i, argv[i]);

	return 0;
}
