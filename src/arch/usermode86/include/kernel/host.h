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

#include <uservisor_base.h>

struct host_timeval {
	int tv_sec;
	int tv_usec;
};

#define HOST_O_RDONLY   00000000
#define HOST_O_WRONLY   00000001
#define HOST_O_RDWR     00000002
#define HOST_O_NONBLOCK	00004000

#define HOST__FD_SETSIZE 1024
#define HOST__NFDBITS (8 * sizeof(unsigned long))
#define HOST__FDSET_LONGS (HOST__FD_SETSIZE/HOST__NFDBITS)

typedef struct {
	     unsigned long fds_bits [HOST__FDSET_LONGS];
} host_fd_set;

#define HOST_FD_BITWORD(f) \
	((f) / HOST__NFDBITS)

#define HOST_FD_BITMASK(f) \
	(1 << ((f) % HOST__NFDBITS))

#define HOST_FD_SET(fd, set) \
	do {                                                     \
		(set)->fds_bits[HOST_FD_BITWORD(fd)] |= HOST_FD_BITMASK(fd); \
	} while(0)

#define HOST_FD_ZERO(s) \
	do {                                          \
		int _i;                               \
		for(_i = 0; _i < HOST__FDSET_LONGS; _i++) { \
			(s)->fds_bits[_i] = 0;        \
		}                                     \
	} while(0)

/* Tests to see if a file descriptor is part of the set */
#define HOST_FD_ISSET(f, s) \
	((s)->fds_bits[HOST_FD_BITWORD(f)] & HOST_FD_BITMASK(f))

/**  Remove a given file descriptor from a set */
#define HOST_FD_CLR(f, s) \
	do {                                                        \
		(s)->fds_bits[HOST_FD_BITWORD(f)] &= (~HOST_FD_BITMASK(f)); \
	} while(0)


#define NR_EXIT   1
#define NR_FORK   2
#define NR_READ   3
#define NR_WRITE  4
#define NR_OPEN   5
#define NR_CLOSE  6
#define NR_GETPID 20
#define NR_PAUSE  29
#define NR_KILL   37
#define NR_PIPE   42
#define NR_SIGNAL 48
#define NR_DUP2   63
#define NR_SELECT 142

#define HOST_SIGUSR1 10

extern int host_read(int fd, void *buf, int len);

extern int host_write(int fd, const void *buf, int len);

extern int host_open(const char *name, int flags, int mode);

extern int host_kill(host_pid_t pid, int signal);

extern int host_signal(int signum, host_sighandler_t handler);

extern int host_pipe(int *pipe);

extern int host_dup2(int fd, int fd2);

extern void host_exit(int ret);

extern host_pid_t host_fork(void);

extern int host_close(int fd);

extern host_pid_t host_getpid(void);

extern int host_pause(void);

extern int host_select(int nfds, host_fd_set *readfds, host_fd_set *writefds,
		host_fd_set *exceptfds, struct host_timeval *timeout);

#endif /* ARCH_USERMODE86_HOST_H_ */
