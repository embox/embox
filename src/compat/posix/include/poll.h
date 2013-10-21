/**
 * @file
 * @brief
 *
 * @date 21.10.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_POLL_H_
#define COMPAT_POSIX_POLL_H_

#define POLLIN     0x01
#define POLLRDNORM POLLIN
#define POLLRDBAND POLLIN
#define POLLPRI    POLLIN
#define POLLOUT    0x02
#define POLLWRNORM POLLOUT
#define POLLWRBAND POLLOUT
#define POLLERR    0x04
#define POLLHUP    0x08
#define POLLNVAL   0x10

/* Data structure describing a polling request */
struct pollfd {
	int fd;        /* file descriptor */
	short events;  /* requested events */
	short revents; /* returned events */
};

/* Type used for the number of file descriptors */
typedef unsigned long int nfds_t;

extern int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif /* COMPAT_POSIX_POLL_H_ */
