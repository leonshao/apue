/*
 * lseek_stdin.c
 *
 *  Created on: 2017年2月25日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	if(lseek(STDIN_FILENO, 0, SEEK_SET) == -1)
		printf("cannot seek\n");
	else
		printf("seek OK\n");
	return 0;
}
