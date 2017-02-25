/*
 * stdin_to_stdout.c
 *
 *  Created on: 2017年2月25日
 *      Author: leon
 */

#include "apue.h"
#include "file_io.h"

int main(void) {
	int n, count = 0;
	char buf[524288];	/* change BUFFSIZE for different time cost trial */

	set_fl(STDOUT_FILENO, O_SYNC);
	while((n = read(STDIN_FILENO, buf, 524288)) > 0) {
		count++;
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
//		fsync(STDOUT_FILENO);
	}

	if(n < 0)
		err_sys("read error");

	err_quit("read count: %d\n", count);

	return 0;
}
