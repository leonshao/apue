/*
 * send_err.c
 *
 *  Created on: 2015年7月29日
 *      Author: leon
 */

#include "apue.h"

/*
 * Used when we had planned to send an fd using send_fd(),
 * but encountered an error instead. We send the error back
 * using the send_fd()/recv_fd() protocol
 */
int send_err(int fd, int status, const char *msg) {
	int n;
	if((n = strlen(msg)) > 0)
		if(write(fd, msg, n) != n)	/* send the error msg */
			return -1;

	if(status >= 0)
		status = -1;	/* must be negative */

	if(send_fd(fd, status) < 0)
		return -1;

	return 0;
}
