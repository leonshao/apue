/*
 * add2.c
 *
 *  Created on: 2015年3月29日
 *      Author: leon
 */

#include "apue.h"

int main(void)
{
	int 	n, int1, int2;
	char	line[MAXLINE];
	char	*invalid_str = "invalid args\n";

	while((n = read(STDIN_FILENO, line, MAXLINE)) > 0)
	{
		line[n] = 0;	/* null terminate */
		if(sscanf(line, "%d%d", &int1, &int2) == 2)	/* should use &int1 */
		{
			snprintf(line, MAXLINE, "%d\n", int1 + int2);
			n = strlen(line);
			if(write(STDOUT_FILENO, line, n) != n)
				err_sys("write error");
		}
		else
		{
			n = strlen(invalid_str);
			if(write(STDOUT_FILENO, invalid_str, n) != n)
				err_sys("write error");
		}
	}

	return 0;
}
