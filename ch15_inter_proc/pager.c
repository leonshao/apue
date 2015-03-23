/*
 * pager.c
 *
 *  Created on: 2015年3月16日
 *      Author: leon
 */

#include "apue.h"

#define DEFAULT_PAGER	"/bin/more"		/* default pager program */

int main(int argc, char *argv[])
{
	FILE 	*fp;
	int		fd[2];
	pid_t	pid;
	char	line[MAXLINE];
	int		len;
	char	*pager, *argv0;

	if (argc != 2)
		err_quit("usage: %s <pathname>", argv[0]);

	/* open file from argv[1] */
	if((fp = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);

	if(pipe(fd) < 0)
		err_sys("pipe error!");

	if((pid = fork()) < 0)
		err_sys("fork error!");
	else if(pid > 0)		/* parent */
	{
		close(fd[0]);

		/* parent copies file to pipe by lines */
		while(fgets(line, MAXLINE, fp) != NULL)
		{
			len = strlen(line);
			if(write(fd[1], line, len) != len)
				err_sys("write error to pipe");
		}

		if (ferror(fp))
			err_sys("fgets error");

		close(fd[1]);

		if(waitpid(pid, NULL, 0) < 0)
			err_sys("waitpid error");
	}
	else					/* child */
	{
		close(fd[1]);

		if(fd[0] != STDIN_FILENO)
		{
			/* copy fd[0] to STDIN_FILENO,
			 * STDIN_FILENO becomes pipe input */
			if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error");
			close(fd[0]);	/* don't need fd[0] after dup2 */
		}

		/* get arguments for execl() */
		if((pager = getenv("PAGER")) == NULL)
			pager = DEFAULT_PAGER;

		/* /bin/more
		 *      ^
		 *    argv0
		 */
		if((argv0 = strrchr(pager, '/')) != NULL)
			++argv0;	/* step past rightmost slash */
		else
			argv0 = pager;

		printf("child execl pager\n");
		/* now data input from pipe instead of std input */
		if(execl(pager, argv0, (char *)0) < 0)
			err_sys("execl error for %s", pager);
	}

	exit(0);
}
