/**
 * @file
 *
 * @brief Socket interface function for kernel mode
 *
 * @date 08.02.2010
 * @author Nikolay Korotky
 */
#ifndef KERNEL_SOCKET_H_
#define KERNEL_SOCKET_H_

#include <net/socket.h>
#include <net/net.h>

extern void kernel_sock_release(struct socket *sock);
extern int kernel_sock_init(void);
extern int sock_create_kern(int family, int type, int protocol,
					struct socket **res);
extern int kernel_bind(struct socket *sock, struct sockaddr *addr, int addrlen);
extern int kernel_listen(struct socket *sock, int backlog);
extern int kernel_accept(struct socket *sock, struct socket **newsock,
					int flags);
extern int kernel_connect(struct socket *sock, struct sockaddr *addr,
					int addrlen, int flags);
extern int kernel_getsockname(struct socket *sock, struct sockaddr *addr,
					int *addrlen);
extern int kernel_getpeername(struct socket *sock, struct sockaddr *addr,
					int *addrlen);
extern int kernel_getsockopt(struct socket *sock, int level, int optname,
					char *optval, int optlen);
extern int kernel_setsockopt(struct socket *sock, int level, int optname,
					char *optval, int optlen);
extern int kernel_sendmsg(struct kiocb *iocb, struct socket *sock,
				struct msghdr *m, size_t total_len);
extern int kernel_recvmsg(struct kiocb *iocb, struct socket *sock,
				struct msghdr *m, size_t total_len, int flags);
#if 0
extern int kernel_sendpage(struct socket *sock, struct page *page, int offset,
			size_t size, int flags);
extern int kernel_sock_ioctl(struct socket *sock, int cmd, unsigned long arg);
#endif

/**
 * Go from a socket descriptor to its socket slot
 */
extern struct socket *sockfd_lookup(int fd);
extern int sock_get_fd(struct socket *sock);

#endif /* KERNEL_SOCKET_H_ */
