/*
 * stdio_buff_stat.c
 *
 *  Created on: 2017年2月15日
 *      Author: leon
 */

#include "apue.h"

void pr_stdio(const char *, FILE *);

int main(void) {
	FILE *fp;
	/*
	 * allocate buffer to stdout, stdin
	 */
	fputs("enter any character\n", stdout);
	if(getchar() == EOF)
		err_sys("getchar error");
	fputs("one line to standard error\n", stderr);

	pr_stdio("stdin", stdin);
	pr_stdio("stdout", stdout);
	pr_stdio("stderr", stderr);

	if((fp = fopen("/etc/hosts", "r")) == NULL)
		err_sys("fopen error");
	if(getc(fp) == EOF)
		err_sys("getc error");
	pr_stdio("/etc/hosts", fp);

	sleep(10);

	return 0;
}

void pr_stdio(const char *name, FILE *fp) {
	printf("stream = %s, ", name);

	/*
	 * The following is nonportable.
	 */
	if(fp->_IO_file_flags & _IO_UNBUFFERED)
		printf("unbuffered");
	else if(fp->_IO_file_flags & _IO_LINE_BUF)
		printf("line buffered");
	else /* if neither of above */
		printf("fully buffered");
	printf(", buffer size = %ld\n", fp->_IO_buf_end - fp->_IO_buf_base);
}