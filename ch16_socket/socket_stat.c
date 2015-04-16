/*
 * socket_stat.c
 *
 *  Created on: 2015年4月16日
 *      Author: leon
 */

#include "apue.h"

int main(int argc, char *argv[])
{
	int 			sockfd, fd;
	struct stat 	sockstat, filestat;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		err_sys("socket error");

	if(fstat(sockfd, &sockstat) < 0)
		err_sys("fstat error");

	printf("sockstat.st_dev: %lu\n", sockstat.st_dev);
	printf("sockstat.st_ino: %lu\n", sockstat.st_ino);
	printf("sockstat.st_nlink: %lu\n", sockstat.st_nlink);
	printf("sockstat.st_mode: %u\n", sockstat.st_mode);
	printf("sockstat.st_uid: %u\n", sockstat.st_uid);
	printf("sockstat.st_gid: %u\n", sockstat.st_gid);
	printf("sockstat.st_rdev: %lu\n", sockstat.st_rdev);
	printf("sockstat.st_size: %ld\n", sockstat.st_size);
	printf("sockstat.st_blksize: %ld\n", sockstat.st_blksize);
	printf("sockstat.st_blocks: %ld\n", sockstat.st_blocks);
	printf("sockstat.st_atime: %ld\n", sockstat.st_atime);
	printf("sockstat.st_mtime: %ld\n", sockstat.st_mtime);
	printf("sockstat.st_ctime: %ld\n", sockstat.st_ctime);

	printf("\n");

	if((fd = open(argv[0], O_RDONLY)) < 0)
		err_sys("open error");

	if(fstat(fd, &filestat) < 0)
		err_sys("fstat error");

	printf("filestat.st_dev: %lu\n", filestat.st_dev);
	printf("filestat.st_ino: %lu\n", filestat.st_ino);
	printf("filestat.st_nlink: %lu\n", filestat.st_nlink);
	printf("filestat.st_mode: %u\n", filestat.st_mode);
	printf("filestat.st_uid: %u\n", filestat.st_uid);
	printf("filestat.st_gid: %u\n", filestat.st_gid);
	printf("filestat.st_rdev: %lu\n", filestat.st_rdev);
	printf("filestat.st_size: %ld\n", filestat.st_size);
	printf("filestat.st_blksize: %ld\n", filestat.st_blksize);
	printf("filestat.st_blocks: %ld\n", filestat.st_blocks);
	printf("filestat.st_atime: %ld\n", filestat.st_atime);
	printf("filestat.st_mtime: %ld\n", filestat.st_mtime);
	printf("filestat.st_ctime: %ld\n", filestat.st_ctime);
	return 0;
}
