/*
 * uptime_server_fork.c
 *
 *  Created on: 2015年4月28日
 *      Author: leon
 */

#include "apue.h"

#define QLEN 10

void serv(int sockfd)
{
	int 				clientfd;
	struct sockaddr_in	client_addr;
	socklen_t			alen;
	char				addrbuf[INET_ADDRSTRLEN];
	const char			*p_sa;
	pid_t				pid;
	int					status;

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

		if((pid = fork()) < 0)
		{
			/*
			 * once daemonized, no msg is output to console.
			 * use syslog for ouput log instead
			 */
			err_sys("fork error");
		}
		else if(pid == 0)	/* child */
		{
			if (dup2(clientfd, STDOUT_FILENO) != STDOUT_FILENO ||
				dup2(clientfd, STDERR_FILENO) != STDERR_FILENO)
			{
				err_sys("dup2 error");
			}
			close(clientfd);
			execl("/usr/bin/uptime", "uptime", (char *)0);
			err_sys("execl error");
		}
		else				/* parent */
		{
			printf("child pid: %d\n", pid);
			close(clientfd);
			waitpid(pid, &status, 0);
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

	/*
	 * host got from local, so only accept local request,
	 * this can be verified by netstat -tlpn, output as below:
	 *
	 * Proto Recv-Q Send-Q Local Address           Foreign Address         State
	 * tcp        0      0 127.0.1.1:36666         0.0.0.0:*               LISTEN
	 */
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

	init_addrinfo(&hint);
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
	 * Because getaddrinfo without service,
	 * the port of returned addrinfo is set to zero.
	 * Need to set an unused port.
	 */
	sa_in.sin_port = htons(port);

	/*
	 * !!!
	 * pay attention to the alen, do not input by sizeof(struct sockaddr)
	 * because the struct size depends on the machine archtect
	 */
	if((sockfd = init_server(SOCK_STREAM, (struct sockaddr *)&sa_in,
			INET_ADDRSTRLEN, QLEN)) >= 0)
	{
		serv(sockfd);
		exit(0);
	}
	else
		err_sys("init_server error %s", strerror(errno));

	return 0;
}
