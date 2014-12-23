/*
 * nonblk_write.c
 *
 *  Created on: 2014年12月8日
 *      Author: leon
 */
#include "apue.h"
#include "file_io.h"	// for set_fl, clr_fl
#include <errno.h>		// for errno
#include <fcntl.h>		// for O_NONBLOCK

char	buf[500000];

int main(void)
{
	int 	ntowrite, nwrite;
	char	*ptr;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", ntowrite);

	set_fl(STDOUT_FILENO, O_NONBLOCK);	/* set nonblocking */

	ptr = buf;
	while(ntowrite > 0)
	{
		errno = 0;

		/*
		 * Write up to ntowrite bytes to the nonblocking
		 * standard output. If the output is not complete,
		 * the write returns -1 and errno is set to 11(EAGAIN).
		 * So it takes many times calling write() to write
		 * all the bytes.
		 */
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d, msg: %s\n",
				nwrite, errno, strerror(errno));
		if (nwrite > 0)
		{
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}

	clr_fl(STDOUT_FILENO, O_NONBLOCK);	/* clear nonblocking */

	exit(0);
}
