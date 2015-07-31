/*
 * open_server2.c
 *
 *  Created on: 2015年7月31日
 *      Author: leon
 */

#include "apue.h"
#include <sys/time.h>
#include <sys/select.h>

int 	debug, log_to_stderr;
char	errmsg[MAXLINE];
int		oflag;
char	*pathname;

void loop(void);

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

	loop();

	return 0;	/* should never returns */
}

#define NALLOC	10	/* # client structs to alloc/realloc for */
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
	else
		client = realloc(client, NEWNALLOC * sizeof(Client));
	if(client == NULL)
		err_sys("can't alloc for client array");

	/* initialize the new entries */
	for(i = client_size; i < NEWNALLOC; ++i)
		client[i].fd = -1;	/* fd of -1 means entry available */

	client_size += NALLOC;
	err_msg("client array full or empty, new size: %d", NEWNALLOC);
}

/*
 * Called by loop() when connection request from a new client arrives.
 */
int client_add(int fd, uid_t uid) {
	int		i;

	if (client == NULL)		/* first time we're called */
		client_alloc();

again:
	for(i = 0; i < client_size; ++i) {
		if(client[i].fd == -1) {
			client[i].fd 	= fd;
			client[i].uid 	= uid;
			return i;	/* return index in client[] array */
		}
	}

	/* client array full, time to realloc for more */
	client_alloc();
	goto again;
}

/*
 * Called by loop() when we're done with a client.
 */
void client_del(int fd) {
	int		i;

	if(client == NULL)
		return;

	for(i = 0; i < client_size; ++i) {
		if(client[i].fd == fd) {
			client[i].fd = -1;
			return;
		}
	}

	err_quit("can't find client entry for fd %d", fd);
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
			continue;
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
