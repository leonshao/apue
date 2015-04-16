/*
 * gethostent.c
 *
 *  Created on: 2015年4月16日
 *      Author: leon
 */

#include "apue.h"

/*
 * hostent data comes from file /etc/hosts
 */
int main(void)
{
	struct hostent	*p_ent;
	int				hostnum = 1;
	int				aliasnum = 1;
	int				addrnum = 1;
	char			**p_aliases;
	char			**p_addr;
	char			addrbuf[INET_ADDRSTRLEN];

	while((p_ent = gethostent()) != NULL)
	{
		printf("hostent_%d:\n", hostnum);
		printf("h_name: %s\n", p_ent->h_name);
		for(p_aliases = p_ent->h_aliases; *p_aliases; ++aliasnum, ++p_aliases)
		{
			printf("alias_%d:\n", aliasnum);
			printf("  %s\n", *p_aliases);
		}

		printf("h_addrtype: %d\n", p_ent->h_addrtype);
		printf("h_length: %d\n", p_ent->h_length);
		for(p_addr = p_ent->h_addr_list; *p_addr; ++addrnum, ++p_addr)
		{
			printf("addr_%d:\n", addrnum);
			/*
			 * network address is stored in binary format(32bits),
			 * needs inet_ntop to convert the address to text form
			 */
			printf("  %s\n", inet_ntop(AF_INET, *p_addr, addrbuf, INET_ADDRSTRLEN));
		}

		printf("\n");

		++hostnum;
	}

	endhostent();

	return 0;
}
