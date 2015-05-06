/*
 * client.c
 *
 *  Created on: 2015年4月23日
 *      Author: leon
 */

#include "apue.h"

#define MAXSLEEP	128

void print_response(int sockfd)
{
	int		n;
	char	buf[BUFLEN];

	while((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
		write(STDOUT_FILENO, buf, n);

	if(n < 0)
		err_sys("recv error: %s", strerror(errno));
}

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
	int 				nsec;
	char				buf[BUFLEN];
	struct sockaddr_in	*p_sa_in = (struct sockaddr_in *)addr;
	const char			*p_sa;

	for(nsec = 1; nsec < MAXSLEEP; nsec <<= 1)
	{
		if(connect(sockfd, addr, alen) == 0)
			return 0;

		p_sa = inet_ntop(AF_INET, &(p_sa_in->sin_addr.s_addr), buf, BUFLEN);
		printf("retry to connect %s\n", p_sa);

		if(nsec < (MAXSLEEP / 2))
			sleep(nsec);
	}

	return -1;
}

int main(int argc, char *argv[])
{
	int					sockfd, ret;
	char				*host;
	uint16_t 			port = DEFAULT_PORT;
	struct addrinfo		*p_addrlist = NULL;
	struct addrinfo		hint;
	struct sockaddr_in 	sa_in;

	if(argc == 1)
	{
		host = get_defaulthost();
	}
	if(argc >= 2)
	{
		host = argv[1];
	}
	if(argc >= 3)
	{
		port = (uint16_t)atoi(argv[2]);
	}

	/*
	 * get remote addr from input host or default host
	 */
	init_addrinfo(&hint, SOCK_STREAM);
	if((ret = getaddrinfo(host, NULL, &hint, &p_addrlist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(ret));

	if(p_addrlist != NULL)
		memcpy((void *)&sa_in, (void *)p_addrlist->ai_addr, sizeof(sa_in));
	else
	{
		sa_in.sin_family = AF_INET;
		sa_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	}

	sa_in.sin_port = htons(port);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error: %s", strerror(errno));

	/*
	 * !!!
	 * pay attention to the alen, it is not correct by input sizeof(struct sockaddr)
	 * because the struct size depends on the machine archtect
	 */
	if(connect_retry(sockfd, (struct sockaddr *)&sa_in, INET_ADDRSTRLEN) < 0)
		err_quit("connect fail: %s", strerror(errno));

	print_response(sockfd);

	return 0;
}
