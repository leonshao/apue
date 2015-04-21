/*
 * uptime_server.c
 *
 *  Created on: 2015年4月21日
 *      Author: leon
 */

#include "apue.h"

/*
 * HOST_NAME_MAX is always defined in bits/local_lim.h
 */
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

#define DEFAULT_PORT 36666
#define QLEN 10
#define BUFLEN 128


static char *get_defaulthost()
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

static void init_addrinfo(struct addrinfo *hint)
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

void serv(int sockfd)
{
	int 				clientfd;
	FILE				*fp;
	char				buf[BUFLEN];
	struct sockaddr_in	client_addr;
	socklen_t			alen;
	char				addrbuf[INET_ADDRSTRLEN];
	const char			*p_sa;

	for(;;)
	{
		clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &alen);
		if(clientfd < 0)
			err_sys("accept error %s", strerror(errno));
		else
		{
			p_sa = inet_ntop(AF_INET, &client_addr.sin_addr.s_addr,
					addrbuf, INET_ADDRSTRLEN);
			printf("get connection from: %s:%d\n", p_sa, client_addr.sin_port);
		}

		if((fp = popen("/usr/bin/uptime", "r")) == NULL)
		{
			snprintf(buf, BUFLEN, "error: %s\n", strerror(errno));
			send(clientfd, buf, strlen(buf), 0);
		}
		else
		{
			while(fgets(buf, BUFLEN, fp) != NULL)
				send(clientfd, buf, strlen(buf), 0);

			pclose(fp);
		}

		close(clientfd);
	}
}

int main(int argc, char *argv[])
{
	int 				sockfd, ret;
	char				*host;
	struct addrinfo		*p_addrlist = NULL;
	struct addrinfo		hint;
	struct sockaddr_in	sa_in;

	host = get_defaulthost();

	init_addrinfo(&hint);
	if((ret = getaddrinfo(host, NULL, NULL, &p_addrlist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(ret));

	if(p_addrlist != NULL)
		memcpy((void *)&sa_in, (void *)p_addrlist->ai_addr, sizeof(sa_in));
	/*
	 * Because getaddrinfo without service,
	 * the port of returned addrinfo is set to zero.
	 * Need to set an unused port.
	 */
	sa_in.sin_port = htons(DEFAULT_PORT);

	if((sockfd = init_server(SOCK_STREAM, (struct sockaddr *)&sa_in,
			sizeof(struct sockaddr), QLEN)) >= 0)
	{
		serv(sockfd);
		exit(0);
	}
	else
		err_sys("init_server error %s", strerror(errno));

	return 0;
}
