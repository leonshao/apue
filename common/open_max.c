/*
 * open_max.c
 *
 *  Created on: 2015年3月24日
 *      Author: leon
 */

#include "apue.h"
#include <limits.h>

#ifdef	OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

/*
 * If OPEN_MAX is indeterminate, we're not
 * guaranteed that this is adequate
 */
#define	OPEN_MAX_GUESS	256

long open_max(void)
{
	if(openmax == 0)
	{
		errno = 0;
		if((openmax = sysconf(_SC_OPEN_MAX)) < 0)
		{
			if(errno == 0)
				/* error in sysconf, OPEN_MAX is indeterminate */
				openmax = OPEN_MAX_GUESS;
			else
				err_sys("sysconf error for _SC_OPEN_MAX");
		}
	}

	return openmax;
}
