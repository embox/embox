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

struct host_timeval {
	int tv_sec;
	int tv_usec;
};

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
#define NR_CLOSE  6
#define NR_KILL   37
#define NR_PIPE   42
#define NR_SELECT 142

#define SIGUSR1 10

enum emvisor_msg {
	EMVISOR_DIAG_OUT = 0,
};

struct emvisor_msghdr {
	enum emvisor_msg type;
	int dlen;
};

extern int embox_getupstream(void);
extern int embox_getdownstream(void);

extern int emvisor_send(int fd, enum emvisor_msg type, void *msg_data, int dlen);
extern int emvisor_recv(int fd, struct emvisor_msghdr *msg, void *data, int dlen);

extern int host_read(int fd, void *buf, int len);

extern int host_write(int fd, void *buf, int len);

extern int host_kill(host_pid_t pid, int signal);

extern int host_pipe(int *pipe);

extern void host_exit(int ret);

extern host_pid_t host_fork(void);

extern int host_close(int fd);

extern int host_select(int nfds, host_fd_set *readfds, host_fd_set *writefds,
		host_fd_set *exceptfds, struct host_timeval *timeout);

#endif /* ARCH_USERMODE86_HOST_H_ */
