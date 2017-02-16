/*
 * tmpnam.c
 *
 *  Created on: 2017年2月16日
 *      Author: leon
 */

#include "apue.h"

int main(void) {
	char name[L_tmpnam], line[MAXLINE];
	FILE *fp;

	printf("%s\n", tmpnam(NULL));	/* first temp name */
	tmpnam(name);					/* second temp name */
	printf("%s\n", name);

	if((fp = tmpfile()) == NULL)
		err_sys("tmpfile error");
	fputs("one line of output\n", fp);	/* write to temp file */
	rewind(fp);							/* then read it back */
	if(fgets(line, MAXLINE, fp) == NULL)
		err_sys("fgets error");

	fputs(line, stdout);			/* print the line we wrote */

	return 0;
}
