/*
 * getaddrinfo.c
 *
 *  Created on: 2015年4月20日
 *      Author: leon
 */

#include "apue.h"

void print_flags(struct addrinfo *aip)
{
	int flags = aip->ai_flags;

	printf("flags: ");
	if(flags == 0)
	{
		printf("0, ");
	}
	else
	{
		if(flags & AI_PASSIVE)
			printf("passive, ");
		if(flags & AI_CANONNAME)
			printf("canon, ");
		if(flags & AI_NUMERICHOST)
			printf("numhost, ");

#if defined(AI_NUMERICSERV)
		if(flags & AI_NUMERICSERV)
			printf("numserv, ");
#endif

#if defined(AI_V4MAPPED)
		if(flags & AI_V4MAPPED)
			printf("v4mapped, ");
#endif

#if defined(AI_ALL)
		if(flags & AI_ALL)
			printf("all");
#endif
	}

	printf("\n");
}

void print_family(struct addrinfo *aip)
{
	printf("family: ");
	switch(aip->ai_family)
	{
	case AF_INET:
		printf("inet, ");
		break;
	case AF_INET6:
		printf("inet6, ");
		break;
	case AF_UNIX:
		printf("unix, ");
		break;
	case AF_UNSPEC:
		printf("unspecified, ");
		break;
	default:
		printf("unknown, ");
	}

	printf("\n");
}

void print_socktype(struct addrinfo *aip)
{
	printf("socktype: ");
	switch(aip->ai_socktype)
	{
	case SOCK_STREAM:
		printf("stream");
		break;
	case SOCK_DGRAM:
		printf("datagram");
		break;
	case SOCK_SEQPACKET:
		printf("seqpacket");
		break;
	case SOCK_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_socktype);
	}

	printf("\n");
}

void print_protocol(struct addrinfo *aip)
{
	printf("protocol: ");
	switch(aip->ai_protocol)
	{
	case 0:
		printf("default");
		break;
	case IPPROTO_TCP:
		printf("TCP");
		break;
	case IPPROTO_UDP:
		printf("UDP");
		break;
	case IPPROTO_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_protocol);
	}

	printf("\n");
}

int main(int argc, char *argv[])
{
	struct addrinfo *p_addrlist, *p_ai;
	int				ret;
	char			addrbuf[INET_ADDRSTRLEN];
	const char		*p_sa;
	struct sockaddr_in	*p_sain;

	if(argc != 3)
		err_quit("usage: %s nodename service", argv[0]);

	if((ret = getaddrinfo(argv[1], argv[2], NULL, &p_addrlist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(ret));

	for(p_ai = p_addrlist; p_ai != NULL; p_ai = p_ai->ai_next)
	{
		print_flags(p_ai);
		print_family(p_ai);
		print_socktype(p_ai);
		print_protocol(p_ai);
		printf("host name: %s\n", p_ai->ai_canonname ? p_ai->ai_canonname : "-");
		printf("ai_addrlen: %u\n", p_ai->ai_addrlen);

		if(p_ai->ai_family == AF_INET)
		{
			p_sain = (struct sockaddr_in *)p_ai->ai_addr;
			p_sa = inet_ntop(AF_INET, &p_sain->sin_addr, addrbuf, INET_ADDRSTRLEN);
			printf("address: %s\n", p_sa ? p_sa : "unknown");
			printf("port: %d\n", ntohs(p_sain->sin_port));
		}

		printf("\n");
	}

	return 0;
}
