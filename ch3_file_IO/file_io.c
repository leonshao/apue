/*
 * set_fl.c
 *
 *  Created on: 2014年12月8日
 *      Author: leon
 */

#include "apue.h"	// for err_sys
#include <fcntl.h>

/*
 * Set file flags.
 * flags are file status flsgs to turn on
 */
void set_fl(int fd, int flags)
{
	int val;

	if((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val |= flags;

	if(fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}

/*
 * Clear file flags.
 * flags are file status flsgs to turn off
 */
void clr_fl(int fd, int flags)
{
	int val;

	if((val = fcntl(fd, F_GETFL, 0)) < 0)
		err_sys("fcntl F_GETFL error");

	val &= ~flags;

	if(fcntl(fd, F_SETFL, val) < 0)
		err_sys("fcntl F_SETFL error");
}
