/*
 * client_udp.c
 *
 *  Created on: 2015年5月6日
 *      Author: leon
 */

#include "apue.h"

#define TIMEOUT		5

void sigalrm(int signo)
{
}

void print_response(int sockfd, struct sockaddr *addr, socklen_t alen)
{
	int		n;
	char	buf[BUFLEN];
	struct sockaddr_in	*p_server_addr;
	char				addrbuf[INET_ADDRSTRLEN];

	/*
	 * print remote address info
	 * verify if the address and port are correct.
	 */
	p_server_addr = (struct sockaddr_in *)addr;
	if(inet_ntop(AF_INET, &p_server_addr->sin_addr.s_addr,
			addrbuf, INET_ADDRSTRLEN) == NULL)
		err_sys("inet_ntop error: ", strerror(errno));
	else
		printf("send request to: %s:%d\n", addrbuf, ntohs(p_server_addr->sin_port));

	buf[0] = 0;
	if(sendto(sockfd, buf, 1, 0, addr, alen) < 0)
		err_sys("sendto error: %s", strerror(errno));

	alarm(TIMEOUT);
	if((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0)
	{
		if(errno != EINTR)
			alarm(0);
		err_sys("recvfrom error: %s", strerror(errno));
	}
	alarm(0);
	write(STDOUT_FILENO, buf, n);
}

int main(int argc, char *argv[])
{
	int					sockfd, ret;
	char				*host;
	uint16_t 			port = DEFAULT_PORT;
	struct addrinfo		*p_addrlist = NULL;
	struct addrinfo		hint;
	struct sockaddr_in 	sa_in;
	struct sigaction	sigact;

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

	sigact.sa_handler = sigalrm;
	sigact.sa_flags = 0;
	sigemptyset(&sigact.sa_mask);
	if(sigaction(SIGALRM, &sigact, NULL) < 0)
		err_sys("sigaction error");

	/*
	 * get remote addr from input host or default host
	 */
	init_addrinfo(&hint, SOCK_DGRAM);
	if((ret = getaddrinfo(host, NULL, &hint, &p_addrlist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(ret));

	if(p_addrlist != NULL)
		memcpy((void *)&sa_in, (void *)p_addrlist->ai_addr, p_addrlist->ai_addrlen);
	else
	{
		sa_in.sin_family = AF_INET;
		sa_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	}

	sa_in.sin_port = htons(port);

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		err_sys("socket error: %s", strerror(errno));

	print_response(sockfd, (struct sockaddr *)&sa_in, INET_ADDRSTRLEN);

	return 0;
}
