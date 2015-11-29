/*
 * apue.h
 *
 *  Created on: 2014年12月8日
 *      Author: leon
 */

#ifndef INCLUDE_APUE_H_
#define INCLUDE_APUE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>	/* for waitpid */
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <sys/un.h>

#define	MAXLINE 	4096	/* max line length */

#define FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Error part
 */
void err_ret(const char *, ...);	/* Nonfatal error */
void err_sys(const char *, ...);	/* Fatal error */
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_dump(const char *, ...);
void err_exit(int, const char *, ...);	/* Fatal error, error code */

/*
 * File I/O part
 */
void set_fl(int fd, int flags);
void clr_fl(int fd, int flags);

ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, void *ptr, size_t n);

/*
 * Lock part
 */
/* Needs () for fd, offset, etc. Possibly expressions */
#define read_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))
#define is_read_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len);
int lockfile(int fd);

/*
 * Process part
 */
void pr_exit(int status);

/*
 * Process sync part
 */
void TELL_WAIT(void);
void TELL_PARENT(pid_t pid);
void WAIT_PARENT(void);
void TELL_CHILD(pid_t pid);
void WAIT_CHILD(void);

/*
 * Daemon process part
 */
void daemonize(const char *cmd);

/*
 * Inter process communication part
 */
FILE *my_popen(const char *cmdstring, const char *type);
int my_pclose(FILE *fp);

int s_pipe(int fd[2]);
int serv_listen(const char *name);
int serv_accept(int listenfd, uid_t *uidptr);
int cli_conn(const char *name);

/*
 * Size of control buffer to send/recv one file descriptor
 * CMSG_ALIGN is required for CMSG_LEN just count total
 * bytes of struct cmsghdr with data
 */
#define CONTROLLEN	CMSG_ALIGN(CMSG_LEN(sizeof(int)))
int send_fd(int fd, int fd_to_send);
int send_err(int fd, int status, const char *msg);
int recv_fd(int fd, ssize_t (*userfunc)(int, const void*, size_t));

#define CL_OPEN	"open"				/* client's request for server */
#define CS_OPEN "./opend"	/* server's well-known name */

#define BUFFSIZE	8192
void comm_client(int (*csopen)(char *name, int oflag));
int cl_open(int fd, char *name, int oflag);
void request(char *buf, int nread, int fd);

/*
 * OS standard part
 */
long open_max(void);

/*
 * Network part
 */
/*
 * HOST_NAME_MAX is always defined in bits/local_lim.h
 */
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 	256
#endif

#define DEFAULT_PORT 	36666
#define BUFLEN 			128

char *get_defaulthost();
void init_addrinfo(struct addrinfo *hint, int socktype);
int init_server(int type, const struct sockaddr *addr, socklen_t alen, int qlen);

/*
 * Thread part
 */
int makethread(void *(fn)(void *), void *arg);

/*
 * Signal part
 */
void pr_mask(const char *str);

#endif /* INCLUDE_APUE_H_ */
