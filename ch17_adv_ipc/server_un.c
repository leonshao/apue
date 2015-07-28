/*
 * server_un.c
 *
 *  Created on: 2015年7月28日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	int listenfd, clifd;
	uid_t uid;

	if((listenfd = serv_listen("foo.socket")) < 0)
		err_quit("serv_listen error");

	while((clifd = serv_accept(listenfd, &uid)) > 0) {
		printf("client uid: %d\n", uid);
		if(write(clifd, "ok", 3) < 0)
			err_sys("write error");
		close(clifd);
	}

	/* clifd < 0, error */
	err_quit("serv_accept error");

	return 0;
}
