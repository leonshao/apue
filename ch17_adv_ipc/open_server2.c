/*
 * open_server2.c
 *
 *  Created on: 2015年7月31日
 *      Author: leon
 */

#include "apue.h"
#include <sys/time.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>

int 	debug, log_to_stderr;
char	errmsg[MAXLINE];
int		oflag;
char	*pathname;

void loop(void);
void loop_poll(void);
void loop_epoll(void);

int main(int argc, char *argv[]) {
	int		c;

//	opterr = 0;		/* don't want getopt() writing to stderr */

	while((c = getopt(argc, argv, "d")) != EOF) {
		switch(c) {
		case 'd':	/* debug */
			debug = log_to_stderr = 1;
			break;
		case '?':
			err_quit("unregconized option: -%c", optopt);
			break;
		}
	}

//	if(debug == 0)
//		daemonize("opend");
#ifdef _SYS_EPOLL_H
	loop_epoll();
#else
	loop_poll();
#endif

	return 0;	/* should never returns */
}

#define NALLOC	open_max()	/* # client structs to alloc/realloc for */
#define NEWNALLOC	(client_size + NALLOC)

typedef struct {	/* one Client struct per connected client */
	int		fd;		/* fd, or -1 if available */
	uid_t	uid;
} Client;
Client		*client = NULL;		/* ptr to malloc'ed array */
int			client_size = 0;	/* # entries in client[] array */

/*
 * allocate more entries in the client array
 */
static void client_alloc(void) {
	int 	i;

	if(client == NULL)
		client = malloc(NALLOC * sizeof(Client));
	if(client == NULL)
		err_sys("can't alloc for client array");

	/* initialize the new entries */
	for(i = client_size; i < NEWNALLOC; ++i)
		client[i].fd = -1;	/* fd of -1 means entry available */

	client_size += NALLOC;
	err_msg("client array full or empty, new size: %d", client_size);
}

/*
 * Called by loop() when connection request from a new client arrives.
 */
int client_add(int fd, uid_t uid) {
	if (client == NULL)		/* first time we're called */
		client_alloc();

	if(client[fd].fd != -1)
		err_sys("duplicate fd");

	client[fd].fd 	= fd;
	client[fd].uid 	= uid;

	return fd;
}

/*
 * Called by loop() when we're done with a client.
 */
void client_del(int fd) {
	if(client == NULL)
		return;

	client[fd].fd = -1;
}

void loop(void) {
	int		listenfd, maxfd, maxi, n, clifd, i, nread;
	fd_set	rset, allset;
	uid_t	uid;
	char	buf[MAXLINE];

	FD_ZERO(&allset);

	/* obtain fd to listen fro client request on */
	if((listenfd = serv_listen(CS_OPEN)) < 0)
		err_sys("serv_listen error");
	FD_SET(listenfd, &allset);
	maxfd = listenfd;
	maxi = -1;

	for( ; ; ) {
		rset = allset;	/* rset gets modified each time around */
		if((n = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0)
			err_sys("select error");

		if(FD_ISSET(listenfd, &rset)) {
			/* accept new client request */
			if((clifd = serv_accept(listenfd, &uid)) < 0)
				err_sys("serv_accept error");

			i = client_add(clifd, uid);
			FD_SET(clifd, &allset);
			if(clifd > maxfd)
				maxfd = clifd;
			if(i > maxi)
				maxi = i;

			err_msg("new connection: uid %d, fd %d, maxi %d",
					uid, clifd, maxi);
		}

		for(i = 0; i <= maxi; ++i) {	/* go through client[] array */
			if((clifd = client[i].fd) < 0)
				continue;

			if(FD_ISSET(clifd, &rset)) {
				/* read argument buffer from client */
				if((nread = read(clifd, buf, MAXLINE)) < 0) {
					err_sys("read error on fd %d", clifd);
				} else if(nread == 0) {
					err_msg("closed: uid %d, fd %d", client[i].uid, clifd);
					client_del(clifd);
					FD_CLR(clifd, &allset);
					close(clifd);
				} else
					request(buf, nread, clifd);
			}
		}
	}
}

void loop_poll(void) {
	struct pollfd	*pollfd;
	int				listenfd, maxi, clifd, i, nread;
	uid_t			uid;
	char			buf[MAXLINE];

	if((pollfd = malloc(open_max() * sizeof(struct pollfd))) == NULL)
		err_sys("malloc error");

	/* obtain fd to listen fro client request on */
	if((listenfd = serv_listen(CS_OPEN)) < 0)
		err_sys("serv_listen error");

	client_add(listenfd, 0);	/* we use [0] for listenfd */
	pollfd[0].fd 		= listenfd;
	pollfd[0].events 	= POLLIN;	/* the interesting event types */
	maxi = 0;

	for( ; ; ) {
		if(poll(pollfd, maxi + 1, -1) < 0)
			err_sys("poll error");

		if(pollfd[0].revents & POLLIN) {
			/* accept new client request */
			if((clifd = serv_accept(listenfd, &uid)) < 0)
				err_sys("serv_accept error");

			client_add(clifd, uid);
			++maxi;
			pollfd[maxi].fd 	= clifd;
			pollfd[maxi].events	= POLLIN;

			err_msg("new connection: uid %d, fd %d, maxi %d",
					uid, clifd, maxi);
		}

		/* starts from [1] for [0] is used for listenfd */
		for(i = 1; i<= maxi; ++i) {
			if((clifd = pollfd[i].fd) < 0)
				continue;

			if(pollfd[i].revents & POLLHUP) {
				goto hungup;
			} else if (pollfd[i].revents & POLLIN) {
				/* read argument buffer from client */
				if((nread = read(clifd, buf, MAXLINE)) < 0) {
					err_sys("read error on fd %d", clifd);
				} else if(nread == 0) {
hungup:
					err_msg("closed: uid %d, fd %d", client[clifd].uid, clifd);
					client_del(clifd);	/* client has closed conn */
					pollfd[i].fd = -1;
					if(i == maxi)
						--maxi;
					close(clifd);
				} else	/* process client's request */
					request(buf, nread, clifd);
			}
		}
	}
}

void loop_epoll(void) {
	int					listenfd, epfd, n, i, fd, clifd, nread;
	int					maxnum = open_max();
	struct epoll_event	event, *eventsp;
	uint32_t			events;
	uid_t				uid;
	char				buf[MAXLINE];

	if((epfd = epoll_create(maxnum)) < 0)
		err_sys("epoll_create error");

	if((eventsp = calloc(maxnum, sizeof(struct epoll_event))) == NULL)
		err_sys("calloc error");

	/* obtain fd to listen fro client request on */
	if((listenfd = serv_listen(CS_OPEN)) < 0)
		err_sys("serv_listen error");

	client_add(listenfd, 0);	/* we use [0] for listenfd */
	event.events 	= EPOLLIN;	/* the interesting event types */
	event.data.fd	= listenfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);

	for( ; ; ) {
		if((n = epoll_wait(epfd, eventsp, maxnum, -1)) < 0)
			err_sys("epoll_wait");

		for (i = 0; i < n; ++i) {
			fd 		= eventsp[i].data.fd;
			events	= eventsp[i].events;

			if (events & EPOLLIN) {
				if (fd == listenfd) {
					/* accept new client request */
					if ((clifd = serv_accept(listenfd, &uid)) < 0)
						err_sys("serv_accept error");

					client_add(clifd, uid);
					event.events = EPOLLIN;
					event.data.fd = clifd;
					epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &event);
				}
				else {
					/* read argument buffer from client */
					if((nread = read(fd, buf, MAXLINE)) < 0) {
						err_sys("read error on fd %d", fd);
					} else if(nread == 0) {
						goto hungup;
					} else	/* process client's request */
						request(buf, nread, fd);
				}
			} else if(events & (EPOLLHUP | EPOLLERR)) {
hungup:
				/* close fd only if EPOLLIN was not set */
				err_msg("closed: uid %d, fd %d", client[fd].uid, fd);
				client_del(fd);	/* client has closed conn */
				close(fd);
			}
		}
	}
}

