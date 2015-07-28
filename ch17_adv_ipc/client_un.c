/*
 * client_un.c
 *
 *  Created on: 2015年7月28日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	int fd, n;
	char buf[128] = {0};

	if((fd = cli_conn("foo.socket")) < 0)
		err_quit("cli_conn error");

	while((n = read(fd, buf, sizeof(buf))) > 0)
		printf("%s\n", buf);

	return 0;
}
