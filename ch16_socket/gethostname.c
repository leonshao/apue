/*
 * gethostname.c
 *
 *  Created on: 2015年4月16日
 *      Author: leon
 */

#include "apue.h"

/*
 * HOST_NAME_MAX is always defined in bits/local_lim.h
 */
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

int main(void)
{
	int 	n;
	char	*host;

/*
 * _SC_HOST_NAME_MAX is always defined in bits/confname.h
 */
#ifdef _SC_HOST_NAME_MAX
	n = sysconf(_SC_HOST_NAME_MAX);
	if (n < 0)
#endif
		n = HOST_NAME_MAX;

	if((host = malloc(n)) == NULL)
		err_sys("malloc error");

	if(gethostname(host, n) < 0)
		err_sys("gethostname error");

	printf("host name: %s\n", host);

	return 0;
}
