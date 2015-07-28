/*
 * bind_un.c
 *
 *  Created on: 2015年7月28日
 *      Author: leon
 */

#include "apue.h"
#include <sys/un.h>

/*
 * bind path address to UNIX domain socket
 * print error if path already exists
 */
int main(void) {
	int fd, len;
	struct sockaddr_un un;
	char *path = "foo.socket";
	int path_len = strlen(path);

	un.sun_family = AF_UNIX;
	strncpy(un.sun_path, path, path_len);
	un.sun_path[path_len] = 0;

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");
	len = offsetof(struct sockaddr_un, sun_path) + path_len;
	if (bind(fd, (struct sockaddr *) &un, len) < 0)
		err_sys("bind error");
	printf("UNIX domain socket bound\n");

	return 0;
}
