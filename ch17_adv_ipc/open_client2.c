/*
 * open_client2.c
 *
 *  Created on: 2015年7月31日
 *      Author: leon
 */

#include "apue.h"

int csopen(char *, int);

int main(void) {
	comm_client(csopen);
	return 0;
}

/*
 * Open the file by sending the "name" and "oflag" to the
 * connection server and reading a file descript back
 */
int csopen(char *name, int oflag) {
	static int 	csfd = -1;

	if(csfd < 0) {	/* open connection to conn server */
		if((csfd = cli_conn(CS_OPEN)) < 0)
			err_sys("cli_conn error");
	}

	return cl_open(csfd, name, oflag);
}
