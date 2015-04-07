/*
 * mmap_lock.c
 *
 * use lock to implement semaphore
 *
 *  Created on: 2015年4月7日
 *      Author: leon
 */

#include "apue.h"
#include <sys/mman.h>

#define NLOOPS		10
#define	SIZE		sizeof(long)	/* size of shared memory area */

static int update(long *ptr)
{
	return (*ptr)++;
}

int main(void)
{
	int		fd;
	int		i, counter;
	void	*area;
	pid_t	pid;

	/*
	 * anonymous memory map for related processes
	 */
	if((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED,
			-1, 0)) == MAP_FAILED)
		err_sys("mmap error");

	/* create lock file, such as TELL_WAIT() */
	if ((fd = open("templock", O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
		err_sys("open error");
	if (write(fd, "lock", 4) < 0)
		err_sys("write error");

	/* gets write lock */
	if (writew_lock(fd, 0, SEEK_SET, 0) < 0)
		err_sys("write_lock error");

	if((pid = fork()) < 0)
	{
		err_sys("fork error");
	}
	else if(pid > 0)		/* parent */
	{
		for(i = 0; i < NLOOPS; i += 2)
		{
			if((counter = update((long *)area)) != i)
				err_quit("parent: expected %d, got %d", i, counter);
			printf("parent: counter %d, area %ld\n", counter, *(long *)area);

			if (un_lock(fd, 0, SEEK_SET, 0) < 0)
				err_sys("un_lock error");

			sleep(1);	/* to have child get the lock */

			/* gets write lock */
			if (writew_lock(fd, 0, SEEK_SET, 0) < 0)
				err_sys("writew_lock error");
		}
	}
	else					/* child */
	{
		for(i = 1; i < NLOOPS + 1; i += 2)
		{
			/* gets write lock */
			if (writew_lock(fd, 0, SEEK_SET, 0) < 0)
				err_sys("writew_lock error");

			if((counter = update((long *)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);
			printf("child: counter %d, area %ld\n", counter, *(long *)area);

			if (un_lock(fd, 0, SEEK_SET, 0) < 0)
				err_sys("un_lock error");

			sleep(1);	/* to have parent get the lock */
		}
	}

	return 0;
}
