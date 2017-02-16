/*
 * setbuf.c
 *
 *  Created on: 2017年2月16日
 *      Author: leon
 */

#include "apue.h"

void my_setbuf(FILE *, char *);
void pr_stdio(const char *, FILE *);

int main(void) {
	FILE *fp;
	char buf[BUFSIZ];

	if((fp = tmpfile()) == NULL)
		err_sys("tmpfile error");
	fputs("one line of output\n", fp);
	pr_stdio("tempfile", fp);

	fputs("close file buffer\n", stdout);
	my_setbuf(fp, NULL);
	pr_stdio("tempfile", fp);

	fputs("set file buffer\n", stdout);
	my_setbuf(fp, buf);
	pr_stdio("tempfile", fp);

	return 0;
}

void my_setbuf(FILE *fp, char *buf) {
	if(buf == NULL) {	/* close buf */
		if(setvbuf(fp, buf, _IONBF, 0) != 0)
			err_sys("setvbuf error");
	} else {
		if(setvbuf(fp, buf, _IOFBF, BUFSIZ) != 0)
			err_sys("setvbuf error");
	}
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
