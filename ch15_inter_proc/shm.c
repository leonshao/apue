/*
 * shm.c
 *
 *  Created on: 2015年3月31日
 *      Author: leon
 */

#include "apue.h"
#include <sys/shm.h>
#include <sys/mman.h>

/*
 * +--------------------+ high address
 * |	argv, env		|
 * +--------------------+
 * |	stack			| <- 0x7fff80464d74	(&shmid)
 * +--------------------+
 * |	...				|
 * +--------------------+
 * |					| <- 0x7f11eb4a26a0 (mmptr + MMAP_SIZE)
 * |	mmap memory		|
 * |					| <- 0x7f11eb48a000 (mmptr)
 * |					|
 * |					| <- 0x7f11eb4896a0	(shmptr + SHM_SIZE)
 * |	shared memory	|
 * |					| <- 0x7f11eb471000	(shmptr)
 * +--------------------+
 * |	...				|
 * +--------------------+
 * |					| <- 0xcd06b0		(ptr)
 * |	heap			|
 * |					| <- 0xcb8010		(ptr + MALLOC_SIZE)
 * +--------------------+
 * |	...				|
 * +--------------------+
 * |					| <- 0x60cdc0		(&array[ARRAY_SIZE])
 * |	.bss			|
 * |					| <- 0x603180		(&array[0])
 * +--------------------+
 * |	.data			|
 * +--------------------+
 * |	.text			|
 * +--------------------+ low address
 *
 */

#define ARRAY_SIZE	40000
#define MALLOC_SIZE	100000
#define SHM_SIZE	100000
#define MMAP_SIZE	100000
#define SHM_MODE	0600	/* user read/write, SHM_R | SHM_W */

char	array[ARRAY_SIZE];	/* uninitialized data = bss */

int main(void)
{
	int		shmid;
	char	*mmptr, *ptr, *shmptr;

	/*
	 * print stack address
	 */
	printf("stack address around 0x%lx\n", (unsigned long)&shmid);

	if((mmptr = mmap(0, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED,
			-1, 0)) == MAP_FAILED)
		err_sys("mmap error");
	printf("mmap memory from 0x%lx to 0x%lx\n",
			(unsigned long)mmptr, (unsigned long)(mmptr + MMAP_SIZE));

	/*
	 * print shared memory address
	 */
	if((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0)
		err_sys("shmget error");

	/* pay attention to the return value (void *)-1 descripted by man */
	if((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
		err_sys("shmat error");
	printf("shared memory attached from 0x%lx to 0x%lx\n",
			(unsigned long)shmptr, (unsigned long)(shmptr + SHM_SIZE));

	/*
	 * print heap address
	 */
	if((ptr = malloc(MALLOC_SIZE)) == NULL)
		err_sys("malloc error");
	printf("mallocd from 0x%lx to 0x%lx\n", (unsigned long)ptr,
			(unsigned long)(ptr + MALLOC_SIZE));

	/*
	 * print bss address
	 */
	printf("array[] from 0x%lx to 0x%lx\n", (unsigned long)&array[0],
			(unsigned long)&array[ARRAY_SIZE]);

	if(shmctl(shmid, IPC_RMID, 0) < 0)
		err_sys("shmctl error");

	return 0;
}
