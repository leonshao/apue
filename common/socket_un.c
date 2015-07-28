/*
 * socket_un.c
 *
 *  Created on: 2015年7月28日
 *      Author: leon
 */

#include "apue.h"
#include <time.h>

#define QLEN	10
/*
 * Create a server endpoint of a connection.
 * Return fd if all OK, <0 on error.
 */
int serv_listen(const char *name) {
	int 				fd, len, rval, err;
	struct sockaddr_un 	un;
	int 				name_len = strlen(name);

	/* create a UNIX domain stream socket */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		err_msg("socket error");
		return -1;
	}
	unlink(name); /* in case it already exists */

	/* fill in socket address structure */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strncpy(un.sun_path, name, name_len);
	len = offsetof(struct sockaddr_un, sun_path) + name_len;

	/* bind the name to the descriptor */
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		err_msg("bind error");
		rval = -2;
		goto errout;
	}

	if(listen(fd, QLEN) < 0) {	/* tell kernel we're a server */
		err_msg("listen error");
		rval = -3;
		goto errout;
	}

	return fd;

errout:
	err = errno;
	close(fd);
	errno = err;
	return rval;
}

#define STALE	30	/* client's name can't be older than this (sec) */
/*
 * Wait for a client connection to arrive, and accept it.
 * We also obtain the client's user ID from the pathname
 * that it must bind before calling us.
 * Return new fd if all OK, <0 on error
 */
int serv_accept(int listenfd, uid_t *uidptr) {
	int 				clifd, err, rval;
	socklen_t			len;
	struct sockaddr_un 	un;
	struct stat			statbuf;
	time_t				staletime;

	len = sizeof(un);
	if((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0) {
		err_msg("accept error");
		return -1;
	}
	/* obtain the client's uid from its calling address */
	len -= offsetof(struct sockaddr_un, sun_path); /* len of pathname */
	un.sun_path[len] = 0;

	if(stat(un.sun_path, &statbuf) < 0) {
		err_msg("stat error");
		rval = -2;
		goto errout;
	}

#ifdef	S_ISSOCK
	if(S_ISSOCK(statbuf.st_mode) == 0) {
		err_msg("'%s' is not a socket", un.sun_path);
		rval = -3;
		goto errout;
	}
#endif

	if( (statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
		(statbuf.st_mode & S_IRWXU) != S_IRWXU ) {
		err_msg("'%s' is not rwx------", un.sun_path);
		rval = -4;
		goto errout;
	}

	staletime = time(NULL) - STALE;
	if( statbuf.st_atime < staletime ||
		statbuf.st_ctime < staletime ||
		statbuf.st_mtime < staletime ) {
		err_msg("'%s' i-node is too old", un.sun_path);
		rval = -5;
		goto errout;
	}

	if(uidptr != NULL)
		*uidptr = statbuf.st_uid;	/* return the uid of caller */

	unlink(un.sun_path);	/* we're done with pathname now */

	return clifd;

errout:
	err = errno;
	close(clifd);
	errno = err;
	return rval;
}

#define CLI_PATH	"/var/tmp/"	/* +5 for pid = 14 chars */
#define CLI_PERM	S_IRWXU		/* rwx for user only */
/*
 * Create a client endpoint and connect to a server.
 * Return fd if all OK, <0 on error
 */
int cli_conn(const char *name) {
	int 				fd, err, rval, len;
	struct sockaddr_un 	un;
	int 				name_len = strlen(name);

	/* create a UNIX domain stream socket */
	if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		err_msg("socket error");
		return -1;
	}

	/* fill socket address structure with our address */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	snprintf(un.sun_path, sizeof(un.sun_path), "%s%05d", CLI_PATH, getpid());
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

	unlink(un.sun_path);	/* in case it already exists */
	if(bind(fd, (struct sockaddr *)&un, len) < 0) {
		err_msg("bind error");
		rval = -2;
		goto errout;
	}

	if(chmod(un.sun_path, CLI_PERM) < 0) {
		err_msg("chmod error");
		rval = -3;
		goto errout;
	}

	/* fill socket address structure with server's address */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strncpy(un.sun_path, name, name_len);
	len = offsetof(struct sockaddr_un, sun_path) + name_len;
	if(connect(fd, (struct sockaddr *)&un, len) < 0) {
		err_msg("connect error");
		rval = -4;
		goto errout;
	}

	return fd;

errout:
	err = errno;
	close(fd);
	errno = err;
	return rval;
}

