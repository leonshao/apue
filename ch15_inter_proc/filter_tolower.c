/*
 * filter_tolower.c
 *
 *  Created on: 2015年3月25日
 *      Author: leon
 */

#include "apue.h"
#include <ctype.h>

int main(void)
{
	int		c;

	while((c = getchar()) != EOF)
	{
		if(isupper(c))
			c = tolower(c);

		/*
		 * if invoked by popen,
		 * stdout is duplicated to pipe write end
		 */
		if(putchar(c) == EOF)
			err_sys("output error");

		if(c == '\n')
			fflush(stdout);
	}

	return 0;
}
