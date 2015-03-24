/*
 * popen.c
 *
 *  Created on: 2015年3月23日
 *      Author: leon
 */

#include "apue.h"

#define PAGER	"${PAGER:-more}"	/* default pager program */

int main(int argc, char *argv[])
{
	char	line[MAXLINE];
	FILE	*fpin, *fpout;

	if(argc != 2)
		err_quit("usage: %s <pathname>", argv[0]);
	if((fpin = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);

	/*
	 * set type to "w" for writing
	 * PAGER is executed by shell as below:
	 * sh -c ${PAGER:-more}
	 */
	if((fpout = my_popen(PAGER, "w")) == NULL)
		err_sys("popen error");

	/*
	 * after popen, child fd 0 is set to pipe of type FIFO
	 * this can be sean by lsof -p <childpid>
	 *
	 * COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF   NODE NAME
	 * sh      2998 leon    0r  FIFO    0,8      0t0 124455 pipe
	 */

	/*
	 * read file line by line
	 */
	while(fgets(line, MAXLINE, fpin) != NULL)
	{
		if(fputs(line, fpout) == EOF)
			err_sys("fputs error to pipe");
	}

	if(ferror(fpin))
		err_sys("fgets error");

	/*
	 * parent waits until child finishes
	 */
	if(my_pclose(fpout) < 0)
		err_sys("pclose error");

	return 0;
}
