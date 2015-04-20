/*
 * getservent.c
 *
 *  Created on: 2015年4月20日
 *      Author: leon
 */

#include "apue.h"

/*
 * service data comes from file /etc/services
 */
int main(void)
{
	struct servent 	*p_ent;
	int				servnum = 1;
	int				aliasnum = 1;
	char			**p_aliases;

	while((p_ent = getservent()) != NULL)
	{
		printf("service no: %d\n", servnum);
		printf("s_name: %s\n", p_ent->s_name);

		for(p_aliases = p_ent->s_aliases; *p_aliases; ++aliasnum, ++p_aliases)
		{
			printf("alias_%d:\n", aliasnum);
			printf("  %s\n", *p_aliases);
		}

		printf("s_port: %d\n", p_ent->s_port);
		printf("s_proto: %s\n", p_ent->s_proto);

		printf("\n");
		++servnum;
	}

	return 0;
}
