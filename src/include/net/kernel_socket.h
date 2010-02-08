/**
 * @file kernel_socket.h
 *
 * @brief Socket interface function for kernel mode
 *
 * @date 08.02.2010
 * @author Nikolay Korotky
 */
#include <net/socket.h>
#include <net/net.h>

void kernel_sock_release(struct socket *sock);
int kernel_sock_init(void);
int sock_create_kern(int family, int type, int protocol, struct socket **res);
int kernel_bind(struct socket *sock, struct sockaddr *addr, int addrlen);
int kernel_listen(struct socket *sock, int backlog);
int kernel_accept(struct socket *sock, struct socket **newsock, int flags);
int kernel_connect(struct socket *sock, struct sockaddr *addr,
					    int addrlen, int flags);
int kernel_getsockname(struct socket *sock, struct sockaddr *addr,
					    int *addrlen);
int kernel_getpeername(struct socket *sock, struct sockaddr *addr,
					    int *addrlen);
int kernel_getsockopt(struct socket *sock, int level, int optname,
					    char *optval, int optlen);
int kernel_setsockopt(struct socket *sock, int level, int optname,
					    char *optval, int optlen);

#if 0
int kernel_sendpage(struct socket *sock, struct page *page, int offset,
			size_t size, int flags);
int kernel_sock_ioctl(struct socket *sock, int cmd, unsigned long arg);
#endif
