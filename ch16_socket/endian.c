/*
 * endian.c
 * big-endian or little-endian
 *
 *  Created on: 2015年4月16日
 *      Author: leon
 */

#include "apue.h"

/*
 * +------------------------+ high address
 * |	p: 0x7fff2aff44b0	| <- 0x7fff2aff44b8
 * |------------------------|
 * |	j					|
 * |------------------------|
 * |	i, p[3]: 0			| <- 0x7fff2aff44b3
 * |------------------------|
 * |	i, p[2]: 0			| <- 0x7fff2aff44b2
 * |------------------------|
 * |	i, p[1]: 0			| <- 0x7fff2aff44b1
 * |------------------------|
 * |	i, p[0]: 1			| <- 0x7fff2aff44b0
 * +------------------------+ low address
 */

int main(void)
{
	int i 	= 1;
	char *p	= (char *)&i;
	int j;

	printf("&i: %p, &p: %p, p: %p, p: %d\n", &i, &p, p, *p);
	for(j = 0; j < 4; ++j)
		printf("p[%d](%p): %d\n", j, &p[j], p[j]);

	if(*p > 0)
		printf("little-endian\n");
	else
		printf("big-endian\n");

	return 0;
}
