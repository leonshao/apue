/*
 * getprotoent.c
 *
 *  Created on: 2015年4月20日
 *      Author: leon
 */

#include "apue.h"

/*
 * protocol data comes from file /etc/protocols
 */
int main(void)
{
	struct protoent *p_ent;
	int				protonum = 1;
	int				aliasnum = 1;
	char			**p_aliases;

	while((p_ent = getprotoent()) != NULL)
	{
		printf("protocol no: %d\n", protonum);
		printf("p_name: %s\n", p_ent->p_name);

		for(p_aliases = p_ent->p_aliases; *p_aliases; ++aliasnum, ++p_aliases)
		{
			printf("alias_%d:\n", aliasnum);
			printf("  %s\n", *p_aliases);
		}

		printf("p_proto: %d\n", p_ent->p_proto);

		printf("\n");
		++protonum;
	}

	return 0;
}
