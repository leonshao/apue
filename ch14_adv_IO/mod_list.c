/*
 * mod_list.c
 *
 *  Created on: 2014年12月18日
 *      Author: leon
 */

#include "apue.h"
#include <stropts.h>

int main(int argc, char *argv[])
{
	int					fd, nmods, i;
	struct str_list		list;

	if (argc != 2)
		err_quit("usage: %s <pathname>", argv[0]);

	if ((fd = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open %s", argv[1]);
	if (isastream(fd) == 0)
		err_quit("%s is not a stream", argv[1]);

	/*
	 * Fetch number of modules.
	 */
	if ((nmods = ioctl(fd, I_LIST, (void *)0)) < 0)
		err_sys("I_LIST error for nmods");
	printf("#modules = %d\n", nmods);

	/*
	 * Allocate storage for all the modules names.
	 */
	list.sl_modlist = calloc(nmods, sizeof(struct str_mlist));
	if (list.sl_modlist == NULL)
		err_sys("calloc error");
	list.sl_nmods = nmods;

	/*
	 * Fetch the module names.
	 */
	if (ioctl(fd, I_LIST, &list) < 0)
		err_sys("I_LIST error for list");

	/*
	 * Print the names
	 */
	for (i = 1; i <= nmods; ++i)
		printf("    %s: %s\n", (i == nmods) ? "driver" : "module",
				list.sl_modlist++->l_name);

	exit(0);
}
