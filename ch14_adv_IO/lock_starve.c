/*
 * lock_starve.c
 *
 *  Created on: 2014年12月24日
 *      Author: leon
 */

#include "apue.h"

#define MAX_CHILD	5

int main(int argc, char *argv[])
{
	int 	fd, chdnum;
	pid_t	pid, ppid, chdpid;

	/*
	 * open a file.
	 */
	if ((fd = open("templock", O_RDWR, FILE_MODE)) < 0)
		err_sys("open error");

	/* parent lock a byte with a read lock */
	if (read_lock(fd, 0, SEEK_SET, 1) < 0)
		err_sys("read_lock error");

	ppid = getpid();
	printf("parent[%d] gets read lock\n", ppid);

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)	/* child */
	{
		chdpid = getpid();

		/*
		 * blocked when try to lock the byte
		 * with a write lock
		 */
		printf("child[%d] waits for write lock\n", chdpid);
		if (writew_lock(fd, 0, SEEK_SET, 1) < 0)
			err_sys("child: writew_lock error");

		printf("child[%d] gets write lock\n", chdpid);
		exit(0);
	}
	else				/* parent */
	{
		chdnum = 0;
		/* fork more childs */
		while(1)
		{
			if ((pid = fork()) < 0)
				err_sys("fork error");
			else if (pid == 0)	/* child */
			{
				chdpid = getpid();

				/* child lock a byte with a read lock */
				if (read_lock(fd, 0, SEEK_SET, 1) < 0)
					err_sys("child[%d] read_lock error", chdpid);

				printf("child[%d] gets read lock\n", chdpid);
				exit(0);
			}
			else
			{
				sleep(1);

				if (++chdnum == MAX_CHILD)
				{
					break;
				}
			}
		}

		un_lock(fd, 0, SEEK_SET, 1);
		printf("parent[%d] un_lock\n", ppid);
	}

	exit(0);
}
