/*
 * invoke_filter.c
 *
 *  Created on: 2015年3月25日
 *      Author: leon
 */

#include "apue.h"

int main(void)
{
	char	line[MAXLINE];
	FILE	*fpin;

	/* input to filter_tolower */
	if((fpin = popen("./filter_tolower", "r")) == NULL)
		err_sys("popen error");

	/* after filter_tolower writes to pipe and exit */
	for( ; ; )
	{
		fputs("prompt>", stdout);
		fflush(stdout);

		/* read from pipe */
		if(fgets(line, MAXLINE, fpin) == NULL)
			break;

		if(fputs(line, stdout) == EOF)
			err_sys("fputs error to pipe");
	}

	if(pclose(fpin) < 0)
		err_sys("pclose error");

	putchar('\n');

	return 0;
}
