/*
 * jmp_var.c
 *
 *  Created on: 2016年1月19日
 *      Author: leon
 */

#include "apue.h"
#include <setjmp.h>

static void f1(int, int, int, int);
static void f2(void);

jmp_buf		jmpbuffer;
static int	globval;

int main(void) {
	int				autoval;
	register int	regival;
	volatile int	volaval;
	static int		statval;

	if(setjmp(jmpbuffer) !=0) {
		/*
		 * If compile with -O option, autoval and regival
		 * are optimized to use registers, not mem.
		 * The values are changed after longjmp
		 */
		printf("after longjmp:\n");
		printf("globval = %d, autoval = %d, regival = %d,"
			" volaval = %d, statval = %d\n",
			globval, autoval, regival, volaval, statval);

		return 0;
	}
	/*
	 * Change variables after setjmp, but before longjmp
	 */
	globval = 95; autoval = 96; regival = 97; volaval = 98;
	statval = 99;

	f1(autoval, regival, volaval, statval);
	return 0;
}

static void f1(int i, int j, int k, int l) {
	printf("in f1():\n");
	printf("globval = %d, autoval = %d, regival = %d,"
		" volaval = %d, statval = %d\n",
		globval, i, j, k, l);
	f2();
}

static void f2(void) {
	longjmp(jmpbuffer, 1);
}
