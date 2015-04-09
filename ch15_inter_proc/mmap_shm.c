/*
 * mmap_shm.c
 *
 *  Created on: 2015年4月9日
 *      Author: leon
 */

#include "apue.h"
#include <sys/shm.h>

#define NLOOPS		10
#define	SIZE		sizeof(long)	/* size of shared memory area */
#define SHM_MODE	0600			/* user read/write, SHM_R | SHM_W */

static int update(long *ptr)
{
	return (*ptr)++;
}

int main(void)
{
	int		shmid, i, counter;
	void	*area;
	pid_t	pid;

	/*
	 * use shm instead of mmap
	 */
	if((shmid = shmget(IPC_PRIVATE, SIZE, SHM_MODE)) < 0)
		err_sys("shmget error");

	if((area = shmat(shmid, 0, 0)) == (void *)-1)
		err_sys("shmat error");

	TELL_WAIT();

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

			TELL_CHILD(pid);
			WAIT_CHILD();	/* blocked here until child update the area */
		}
	}
	else					/* child */
	{
		for(i = 1; i < NLOOPS + 1; i += 2)
		{
			WAIT_PARENT();	/* first blocked until parent update the area */

			if((counter = update((long *)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);
			printf("child: counter %d, area %ld\n", counter, *(long *)area);

			TELL_PARENT(getppid());
		}
	}

	return 0;
}
