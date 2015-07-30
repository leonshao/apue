/*
 * recv_fd.c
 *
 *  Created on: 2015年7月29日
 *      Author: leon
 */

#include "apue.h"

static struct cmsghdr *cmptr = NULL;	/* malloc'ed first time */

int recv_fd(int fd, ssize_t (*userfunc)(int, const void*, size_t)) {
	int 			newfd, nr, status;
	struct iovec	iov[1];
	struct msghdr	msg;
	char			buf[MAXLINE];
	char 			*ptr;

	status = -1;
	for( ; ; ) {
		iov[0].iov_base = buf;
		iov[0].iov_len 	= sizeof(buf);
		msg.msg_iov 	= iov;
		msg.msg_iovlen 	= 1;
		msg.msg_name 	= NULL;
		msg.msg_namelen = 0;
		if(cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
			return -1;
		msg.msg_control 	= cmptr;
		msg.msg_controllen	= CONTROLLEN;

		if((nr = recvmsg(fd, &msg, 0)) < 0) {
			err_sys("recvmsg error");
		} else if(nr == 0) {
			err_ret("connection closed by server");
			return -1;
		}

		/*
		 * See if this is the final data with null & status. Null
		 * is next to last byte of buffer; status byte is the last byte.
		 * Zero status means there is a file descriptor to receive
		 */
		for(ptr = buf; ptr < &buf[nr]; ) {
			if(*ptr++ == 0) {
				if(ptr != &buf[nr -1])
					err_dump("message format error");

				status = *ptr & 0xFF;	/* prevent sign extension */
				if(status == 0) {
					if(msg.msg_controllen != CONTROLLEN)
						err_dump("status = 0 but no fd, msg.msg_controllen: %d, "
								"CONTROLLEN: %d", msg.msg_controllen, CONTROLLEN);

					newfd = *(int *)CMSG_DATA(cmptr);
				} else {
					newfd = -status;
				}

				nr -= 2;
			}
		}

		if(nr > 0 && (*userfunc)(STDERR_FILENO, buf, nr) != nr)
			return -1;

		if(status >= 0)
			return newfd;
	}

	return 0;	/* should not reach here */
}
