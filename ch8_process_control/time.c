/*
 * time.c
 *
 *  Created on: 2016年1月8日
 *      Author: leon
 */

#include "apue.h"
#include <sys/times.h>

static void do_cmd(char *);
static void pr_times(clock_t, struct tms *, struct tms *);

int main(int argc, char *argv[]) {
	int		i;

	setbuf(stdout, NULL);
	for(i = 1; i < argc; ++i)
		do_cmd(argv[i]);	/* once for each command-line arg */

	exit(0);
}

static void do_cmd(char *cmd) {	/* execute and time the "cmd" */
	struct tms	timestart, timeend;
	clock_t		start, end;
	int			status;

	printf("\ncommand: %s\n", cmd);

	if((start = times(&timestart)) == -1)	/* starting values */
		err_sys("times error");

	if((status = system(cmd)) < 0)	/* execute command */
		err_sys("system error");

	if((end = times(&timeend)) == -1)	/* ending values */
		err_sys("times error");

	pr_times(end - start, &timestart, &timeend);
	pr_exit(status);
}

static void pr_times(clock_t real, struct tms *timestart, struct tms *timeend) {
	static long		clktck = 0;

	if((clktck = sysconf(_SC_CLK_TCK)) < 0)
		err_sys("sysconf error");

	printf("  real:  %7.3f\n", real / (double) clktck);
	printf("  user:  %7.3f\n",
		(timeend->tms_utime - timestart->tms_utime) / (double) clktck);
	printf("  sys:   %7.3f\n",
		(timeend->tms_stime - timestart->tms_stime) / (double) clktck);
	printf("  child user:  %7.3f\n",
		(timeend->tms_cutime - timestart->tms_cutime) / (double) clktck);
	printf("  child sys:   %7.3f\n",
		(timeend->tms_cutime - timestart->tms_cutime) / (double) clktck);
}
