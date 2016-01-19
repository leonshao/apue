/*
 * atexit.c
 *
 *  Created on: 2016年1月19日
 *      Author: leon
 */

#include "apue.h"

static void my_exit1(void);
static void my_exit2(void);

int main(void){
	if(atexit(my_exit2) != 0)
		err_sys("can't reigster my_exit2");
	if(atexit(my_exit1) != 0)
		err_sys("can't reigster my_exit1");
	if(atexit(my_exit1) != 0)
		err_sys("can't reigster my_exit1");

	printf("main is done\n");

	return 0;
}

static void my_exit1(void) {
	printf("first exit handler\n");
}

static void my_exit2(void) {
	printf("second exit handler\n");
}
