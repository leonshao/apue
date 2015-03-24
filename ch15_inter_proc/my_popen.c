/*
 * my_popen.c
 *
 *  Created on: 2015年3月24日
 *      Author: leon
 */

#include "apue.h"

/*
 * Pointer to array allocated at run time
 */
static pid_t *childpid = NULL;

/*
 * From our open_max()
 */
static int maxfd;

FILE *my_popen(const char *cmdstring, const char *type)
{
	int		pfd[2];		/* for pipe fds */
	pid_t	pid;
	int		i;
	FILE	*fp;

	/* only allow "r" or "w" */
	if( (type[0] != 'r' && type[0] != 'w') || type[1] != 0 )
	{
		errno = EINVAL;
		return NULL;
	}

	if(childpid == NULL)
	{
		/* allocate zeroed out array for child pids */
		maxfd = open_max();
		if((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
			return NULL;
	}

	if(pipe(pfd) < 0)
		return NULL;	/* errno set by pipe() */

	if((pid = fork()) < 0)
		return NULL;	/* errno set by fork() */
	else if(pid == 0)								/* child */
	{
		if(*type == 'r')
		{
			/*
			 * parent reads from child,
			 * child close the no need read fd
			 */
			close(pfd[0]);
			if(pfd[1] != STDOUT_FILENO)
			{
				/* redirect std output to write fd of pipe */
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		}
		else			/* *type == 'w' */
		{
			/*
			 * parent writes to child,
			 * child close the no need write fd
			 */
			close(pfd[1]);
			if(pfd[0] != STDIN_FILENO)
			{
				/* redirect std input to read fd of pipe */
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}

		/*
		 * close all descriptors in childpid[] forked from parent
		 */
		for(i = 0; i < maxfd; ++i)
			if(childpid[i] > 0)
				close(childpid[i]);

		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);

		/* to ensure child exits */
		_exit(127);
	}

	/* parent continues... */
	if (*type == 'r')
	{
		close(pfd[1]);
		/* get file pointer from read fd of pipe */
		if ((fp = fdopen(pfd[0], type)) == NULL)
			return NULL;
	}
	else	/* *type == 'w' */
	{
		close(pfd[0]);
		/* get file pointer from write fd of pipe */
		if ((fp = fdopen(pfd[1], type)) == NULL)
			return NULL;
	}

	/*
	 * Remember child pid for this fd because popen
	 * is possible to be called by parent several times
	 * while child is not finished, thus, several pipes
	 * with fds are created. To close the fp correctly
	 * in pclose(), childpid provides a fd-pid mapping
	 */
	childpid[fileno(fp)] = pid;

	return fp;
}

int my_pclose(FILE *fp)
{
	int 	fd, stat;
	pid_t	pid;

	if(childpid == NULL)
	{
		errno = EINVAL;
		return -1;		/* popen has never been called */
	}

	fd = fileno(fp);
	if((pid = childpid[fd]) == 0)
	{
		errno = EINVAL;
		return -1;		/* fp wasn't opened by popen() */
	}

	childpid[fd] = 0;
	if(fclose(fp) == EOF)
		return -1;

	while(waitpid(pid, &stat, 0) < 0)
		if(errno != EINTR)
			return -1;	/* error other than EINTR from waitpid() */

	return stat;		/* return child's termination status */
}
