/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#ifndef ARCH_USERMODE86_HOST_H_
#define ARCH_USERMODE86_HOST_H_

typedef int host_pid_t;
typedef void (*host_sighandler_t)(int);

typedef unsigned long sigset_t;

struct host_sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
	unsigned long sa_flags;
	void (*sa_restorer)(void);
};

#include <uservisor_base.h>

struct host_timeval {
	int tv_sec;
	int tv_usec;
};

#define HOST_SA_NODEFER	0x40000000

#define HOST_O_RDONLY   00000000
#define HOST_O_WRONLY   00000001
#define HOST_O_RDWR     00000002
#define HOST_O_NONBLOCK	00004000

#define NR_EXIT   1
#define NR_READ   3
#define NR_WRITE  4
#define NR_OPEN   5
#define NR_CLOSE  6
#define NR_GETPID 20
#define NR_PAUSE  29
#define NR_KILL   37
#define NR_DUP2   63
#define NR_SIGACT 67

#define HOST_SIGUSR1 10

extern int host_read(int fd, void *buf, int len);

extern int host_write(int fd, const void *buf, int len);

extern int host_open(const char *name, int flags, int mode);

extern int host_kill(host_pid_t pid, int signal);

extern int host_dup2(int fd, int fd2);

extern void host_exit(int ret);

extern int host_close(int fd);

extern host_pid_t host_getpid(void);

extern int host_pause(void);

extern int host_sigaction(int signum, const struct host_sigaction *new,
		struct host_sigaction *old);

#endif /* ARCH_USERMODE86_HOST_H_ */
