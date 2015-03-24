/*
 * cp_mmap.c
 *
 *  Created on: 2014年12月23日
 *      Author: leon
 */

#include "apue.h"
#include "sys/mman.h"	// for mmap()

int main(int argc, char *argv[])
{
	int 			fdin, fdout;
	struct stat		statbuf;
	void 			*src, *dst;

	if (argc != 3)
		err_quit("usage: %s <fromfile> <tofile>", argv[0]);
	if ((fdin = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open %s for reading", argv[1]);
	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
			FILE_MODE)) < 0)
		err_sys("can't open %s for writing", argv[1]);

	if (fstat(fdin, &statbuf) < 0)	/* need size of input file */
		err_sys("fstat error");

	/* set size of output file */
	if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1)
		err_sys("lseek error");

	/*
	 * Ensure file exists.
	 * Otherwise, SIGBUS occurs when mmap to fdout
	 */
	if (write(fdout, "", 1) != 1)
		err_sys("write error");

	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED,
			fdin, 0)) == MAP_FAILED)
		err_sys("mmap error for input");

	if ((dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, fdout, 0)) == MAP_FAILED)
		err_sys("mmap error for output");

	/*
	 * after mmap succeeds, memcpy works even fdin is closed,
	 */
//	close(fdin);
//	printf("src: %p, dst: %p\n", src, dst);

	memcpy(dst, src, statbuf.st_size);	/* does the file copy */

	exit(0);
}
