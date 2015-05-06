/*
 * uptime_server_udp.c
 *
 *  Created on: 2015年5月5日
 *      Author: leon
 */

#include "apue.h"

#define QLEN 		10
#define MAXADDRLEN	256

void serv(int sockfd)
{
	int					n;
	char				buf[BUFLEN];
	socklen_t			alen;
	struct sockaddr_in	*p_client_addr;
	char				abuf[MAXADDRLEN];
	char				addrbuf[INET_ADDRSTRLEN];
	FILE				*fp;

	for(;;)
	{
		if((n = recvfrom(sockfd, buf, BUFLEN, 0,
						(struct sockaddr *)abuf, &alen)) < 0)
			err_sys("recvfrom error: %s", strerror(errno));

		p_client_addr = (struct sockaddr_in *)&abuf;
		if(inet_ntop(AF_INET, &p_client_addr->sin_addr.s_addr,
				addrbuf, INET_ADDRSTRLEN) == NULL)
			err_sys("inet_ntop error: ", strerror(errno));
		else
			printf("get request from: %s:%d\n", addrbuf, ntohs(p_client_addr->sin_port));

		/*
		 * differs from TCP server, no matter success or error,
		 * use sendto
		 */
		if((fp = popen("/usr/bin/uptime", "r")) == NULL)
		{
			snprintf(buf, BUFLEN, "error: %s\n", strerror(errno));
			sendto(sockfd, buf, strlen(buf), 0,
					(struct sockaddr *)p_client_addr, alen);
		}
		else
		{
			if(fgets(buf, BUFLEN, fp) != NULL)
				sendto(sockfd, buf, strlen(buf), 0,
						(struct sockaddr *)p_client_addr, alen);
			pclose(fp);
		}
	}
}

int main(int argc, char *argv[])
{
	int 				sockfd, ret;
	int					port = DEFAULT_PORT;
	char				*host;
	struct addrinfo		*p_addrlist = NULL;
	struct addrinfo		hint;
	struct sockaddr_in	sa_in;

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

	init_addrinfo(&hint, SOCK_DGRAM);
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

	if((sockfd = init_server(SOCK_DGRAM, (struct sockaddr *)&sa_in,
			INET_ADDRSTRLEN, 0)) >= 0)
	{
		serv(sockfd);
		exit(0);
	}
	else
		err_sys("init_server error %s", strerror(errno));

	return 0;
}
