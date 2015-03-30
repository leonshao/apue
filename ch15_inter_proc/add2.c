/*
 * add2.c
 *
 *  Created on: 2015年3月29日
 *      Author: leon
 */

#include "apue.h"

static char	*invalid_str = "invalid args\n";
static char *printf_err = "printf error";

static void add2_rw(void);
static void add2_fgets(void);

int main(int argc, char *argv[])
{
	if(argc < 2)
		add2_rw();
	else
		add2_fgets();

	return 0;
}

static void add2_rw(void)
{
	int 	n, int1, int2;
	char	line[MAXLINE];

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
}

static void add2_fgets(void)
{
	int 	int1, int2;
	char	line[MAXLINE];

	/*
	 * once stdin and stdout become pipe, they are set to fully buffered
	 * as default, fgets blocks.
	 * use setvbuf to set stdin and stdout to line buffer
	 */
	if(setvbuf(stdin, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");
	if(setvbuf(stdout, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");

	while(fgets(line, MAXLINE, stdin) != NULL)
	{
		if(sscanf(line, "%d%d", &int1, &int2) == 2)	/* should use &int1 */
		{
			if(printf("%d\n", int1 + int2) == EOF)
				err_sys(printf_err);
		}
		else
		{
			if(printf("%s", invalid_str) == EOF)
				err_sys(printf_err);
		}
	}
}

