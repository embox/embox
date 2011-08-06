/**
 * @file
 * @brief An implementation of the SOCKET network access protocol.
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <net/in.h>
#include <net/inet_sock.h>
#include <net/kernel_socket.h>
#include <net/net.h>
#include <net/socket.h>
#include <stddef.h>
#include <types.h>

/* TODO: remove all below from here */

int socket(int domain, int type, int protocol) {
	int res;
	struct socket *sock;

	res = kernel_sock_create(domain, type, protocol, &sock);
	if (res < 0) {
		return res;
	}

	return sock_get_fd(sock);
}

int connect(int sockfd, const struct sockaddr *daddr, socklen_t daddrlen) {
	struct socket *sock;

	sock = sockfd_lookup(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_connect(sock, daddr, daddrlen, 0);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	struct socket *sock;

	sock = sockfd_lookup(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_bind(sock, addr, addrlen);
}

int close(int sockfd) {
	struct socket *sock;

	sock = sockfd_lookup(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_sock_release(sock);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {
	int res;
	struct socket *sock;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	sock = sockfd_lookup(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	iov.iov_base = (void *)buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	inet = inet_sk(sock->sk);
	dest_addr = (struct sockaddr_in *)daddr;
	inet->daddr = dest_addr->sin_addr.s_addr;
	inet->dport = dest_addr->sin_port;
	if (inet->sport == 0) {
		//TODO:
		inet->sport = 666;
	}

	res = kernel_sendmsg(NULL, sock, &m, len);
	if (res < 0) {
		return (ssize_t)res;
	}

	return (ssize_t)len;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
			struct sockaddr *daddr, socklen_t *daddrlen) {
	int res;
	struct socket *sock;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	sock = sockfd_lookup(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	iov.iov_base = buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	res = kernel_recvmsg(NULL, sock, &m, len, flags);
	if (res < 0) {
		return res;
	}

	inet = inet_sk(sock->sk);
	dest_addr = (struct sockaddr_in *)daddr;
	dest_addr->sin_addr.s_addr = inet->daddr;
	dest_addr->sin_port = inet->dport;

	return res;
}
