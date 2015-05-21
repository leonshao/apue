/*
 * uptime_server_multi_port.c
 *
 * multi ports implements the same server function,
 * all response by the uptime
 *
 *  Created on: 2015年5月21日
 *      Author: leon
 */

#include "apue.h"

#define QLEN 10

int ports[] = {22222, 33333, 44444, 55555};

/*
 * no loop in the serv function, it is done by the select part
 * in main function
 */
void serv(int sockfd)
{
	int 				clientfd;
	FILE				*fp;
	char				buf[BUFLEN];
	struct sockaddr_in	client_addr;
	socklen_t			alen;
	char				addrbuf[INET_ADDRSTRLEN];
	const char			*p_sa;

	clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &alen);
	if (clientfd < 0)
		err_sys("accept error %s", strerror(errno));
	else {
		p_sa = inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, addrbuf,
				INET_ADDRSTRLEN);
		printf("get connection from: %s:%d\n", p_sa, client_addr.sin_port);
	}

	if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
		snprintf(buf, BUFLEN, "error: %s\n", strerror(errno));
		send(clientfd, buf, strlen(buf), 0);
	} else {
		while (fgets(buf, BUFLEN, fp) != NULL)
			send(clientfd, buf, strlen(buf), 0);

		pclose(fp);
	}

	close(clientfd);
}

int main(int argc, char *argv[])
{
	int 				sockfd, ret, i, port_size;
	char				*host;
	struct addrinfo		*p_addrlist = NULL;
	struct addrinfo		hint;
	struct sockaddr_in	sa_in;
	int					maxfd = 0, read_fd, n;
	fd_set				sock_fds, read_fds;

	if(argc == 1)
	{
		host = get_defaulthost();
	}
	if(argc >= 2)
	{
		host = argv[1];
	}

	init_addrinfo(&hint, SOCK_STREAM);
	if((ret = getaddrinfo(host, NULL, NULL, &p_addrlist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(ret));

	if(p_addrlist != NULL)
		memcpy((void *)&sa_in, (void *)p_addrlist->ai_addr, sizeof(sa_in));
	else
	{
		sa_in.sin_family = AF_INET;
		sa_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	}

	/*
	 * add sockfd of different ports to fd set
	 * one process listen on multi ports:
COMMAND    PID USER   FD   TYPE DEVICE SIZE/OFF   NODE NAME
uptime_se 4511 leon    3u  IPv4 340266      0t0    TCP ubuntu.ubuntu-domain:22222 (LISTEN)
uptime_se 4511 leon    4u  IPv4 340267      0t0    TCP ubuntu.ubuntu-domain:33333 (LISTEN)
uptime_se 4511 leon    5u  IPv4 340268      0t0    TCP ubuntu.ubuntu-domain:44444 (LISTEN)
uptime_se 4511 leon    6u  IPv4 340269      0t0    TCP ubuntu.ubuntu-domain:55555 (LISTEN)
	 */
	FD_ZERO(&sock_fds);
	port_size = sizeof(ports)/sizeof(int);
	for(i=0; i < port_size; ++i)
	{
		sa_in.sin_port = htons(ports[i]);
		if((sockfd = init_server(SOCK_STREAM, (struct sockaddr *)&sa_in,
					INET_ADDRSTRLEN, QLEN)) < 0)
			err_quit("init_server error: %s", strerror(errno));

		FD_SET(sockfd, &sock_fds);
		if(sockfd > maxfd)
			maxfd = sockfd;
	}

	/*
	 * the socket fd set is passed to select and reset every time
	 * select returns
	 */
	++maxfd;
	for(;;)
	{
		read_fds = sock_fds;
		if((n = select(maxfd, &read_fds, NULL, NULL, NULL)) < 0)
			err_sys("select error: %s", strerror(errno));

		if(n > 0)
		{
			/*
			 * when select returns, there are listening fds ready
			 * to accept, the ready fds are in the read_fds set,
			 * check the set and pass to serv function to process
			 * the connection
			 */
			for (read_fd = 0; read_fd < maxfd; ++read_fd) {
				if (FD_ISSET(read_fd, &read_fds))
					serv(read_fd);
			}
		}
	}

	return 0;
}
