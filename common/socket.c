/*
 * socket.c
 *
 *  Created on: 2015年4月21日
 *      Author: leon
 */

#include "apue.h"

char *get_defaulthost()
{
	int 	n;
	char 	*host;

	/*
	 * _SC_HOST_NAME_MAX is always defined in bits/confname.h
	 */
#ifdef _SC_HOST_NAME_MAX
	n = sysconf(_SC_HOST_NAME_MAX);
	if (n < 0)
#endif
	n = HOST_NAME_MAX;

	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");

	if (gethostname(host, n) < 0)
		err_sys("gethostname error");

	return host;
}

void init_addrinfo(struct addrinfo *hint)
{
	hint->ai_flags 		= 0;
	hint->ai_family		= 0;
	hint->ai_socktype 	= SOCK_STREAM;
	hint->ai_protocol 	= 0;
	hint->ai_addrlen	= 0;
	hint->ai_canonname	= NULL;
	hint->ai_addr		= NULL;
	hint->ai_next		= NULL;
}

/*
 * qlen as backlog for listen()
 */
int init_server(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
	int sockfd;
	int err = 0;

	if((sockfd = socket(addr->sa_family, type, 0)) < 0)
		return -1;

	if(bind(sockfd, addr, alen) < 0)
	{
		err = errno;
		goto errout;
	}

	if(listen(sockfd, qlen) < 0)
	{
		err = errno;
		goto errout;
	}

	return sockfd;

errout:
	close(sockfd);
	errno = err;
	return -1;
}
