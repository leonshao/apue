/*
 * mmap.c
 *
 *  Created on: 2015年4月6日
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
//	int		fd;
	int		i, counter;
	void	*area;
	pid_t	pid;

//	if((fd = open("/dev/zero", O_RDWR)) < 0)
//		err_sys("open error");

//	if((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
//			fd, 0)) == MAP_FAILED)
//		err_sys("mmap error");

	/*
	 * anonymous memory map for related processes
	 */
	if((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED,
			-1, 0)) == MAP_FAILED)
		err_sys("mmap error");

	printf("mmap memory from 0x%lx to 0x%lx\n",
			(unsigned long)area, (unsigned long)(area + SIZE));

//	close(fd);

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
