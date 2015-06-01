#include <sys/types.h>	// for umask
#include <sys/stat.h>
#include <unistd.h>		// for fork
#include <stdlib.h>		// for exit
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdio.h>

/*
 * 守护进程编程规则
 * 1. 调用umask将文件模式创建屏蔽字设为0. 因为继承而来的文件模式屏蔽字可能不适用于守护进程
 * 2. 调用fork之后退出父进程，为调用setsid作准备
 * 3. 调用setsid创建新会话，使守护进程没有控制终端。此时再次调用fork并再次退出父进程，使守护进程不是会话首进程
 *    综合2,3步：fork -> setsid -> fork
 * 4. 更改当前工作目录(一般不常用)
 * 5. 关闭不需要的文件描述符
 * 6. 打开/dev/null,使其具有文件描述符0,1,2. 以防止读标准输入，写标准输出等
 */
void daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	struct rlimit		rl;
	struct sigaction	sa;
	pid_t				pid;

	// 1. Clear file creation mask
	umask(0);

	// Get maximum number of file descriptors
	if(getrlimit(RLIMIT_NOFILE, &rl) < 0)
		return;

	// 2,3. Become a session leader to lose controlling TTY.
	if((pid = fork()) < 0)
		return;
	else if(pid != 0)	/* parent */
		exit(0);
	setsid();

	// Ensure future opens won't allocate controlling TTY.
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
		return;

	if((pid = fork()) < 0)
		return;
	else if(pid != 0)   /* parent again */
		exit(0);

	// Close all open file descriptors.
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; ++i)
		close(i);

	// Attach file descriptors 0, 1, 2 to /dev/null
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	// Initialize the log file.
	openlog(cmd, LOG_CONS | LOG_PID, LOG_DAEMON);

	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d, %d, %d", fd0, fd1,
				fd2);
		exit(1);
	}

	// Log file is /var/log/syslog, log format as below:
	// Nov 11 22:41:11 ubuntu ./a.out[3933]: daemon process starts...
	syslog(LOG_INFO, "daemon process starts...");
}
