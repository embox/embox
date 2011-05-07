/**
 * @file
 * @brief Common inet definitions.
 *
 * @date 09.02.10
 * @author Nikolay Korotky
 */

#ifndef _INET_COMMON_H
#define _INET_COMMON_H

extern int  inet_release(struct socket *sock);
extern int  inet_dgram_connect(struct socket *sock,
				struct sockaddr * uaddr, int addr_len, int flags);
extern int  inet_accept(struct socket *sock,
				struct socket *newsock, int flags);
extern int  inet_sendmsg(struct kiocb *iocb,
				struct socket *sock, struct msghdr *msg, size_t size);
extern int  inet_shutdown(struct socket *sock, int how);
extern int  inet_listen(struct socket *sock, int backlog);
extern void inet_sock_destruct(struct sock *sk);
extern int  inet_bind(struct socket *sock,
				struct sockaddr *uaddr, int addr_len);
extern int  inet_getname(struct socket *sock,
				struct sockaddr *uaddr, int *uaddr_len, int peer);
extern int  inet_ioctl(struct socket *sock,
				unsigned int cmd, unsigned long arg);

#endif /* _INET_COMMON_H */
