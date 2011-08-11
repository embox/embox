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

/* opened sockets in system */
static struct socket * opened_sockets[CONFIG_MAX_KERNEL_SOCKETS] = {0};

int socket(int domain, int type, int protocol) {
	int fd;
	struct socket *sock;

	fd = kernel_socket_create(domain, type, protocol, &sock);
	if (fd < 0) {
		return fd; /* return error code */
	}

	opened_sockets[fd] = sock; /* save socket */

	return fd;
}

int connect(int sockfd, const struct sockaddr *daddr, socklen_t daddrlen) {
	struct socket *sock;

	if ((sockfd < 0) || (sockfd >= CONFIG_MAX_KERNEL_SOCKETS)) {
		return -EBADF;
	}

	sock = opened_sockets[sockfd];
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_socket_connect(sock, daddr, daddrlen, 0);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	struct socket *sock;

	if ((sockfd < 0) || (sockfd >= CONFIG_MAX_KERNEL_SOCKETS)) {
		return -EBADF;
	}

	sock = opened_sockets[sockfd];
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_socket_bind(sock, addr, addrlen);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {
	int res;
	struct socket *sock;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	if ((sockfd < 0) || (sockfd >= CONFIG_MAX_KERNEL_SOCKETS)) {
		return -EBADF;
	}

	sock = opened_sockets[sockfd];
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

	res = kernel_socket_sendmsg(NULL, sock, &m, len);
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

	if ((sockfd < 0) || (sockfd >= CONFIG_MAX_KERNEL_SOCKETS)) {
		return -EBADF;
	}

	sock = opened_sockets[sockfd];
	if (sock == NULL) {
		return -EBADF;
	}

	iov.iov_base = buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	res = kernel_socket_recvmsg(NULL, sock, &m, len, flags);
	if (res < 0) {
		return res;
	}

	inet = inet_sk(sock->sk);
	dest_addr = (struct sockaddr_in *)daddr;
	dest_addr->sin_addr.s_addr = inet->daddr;
	dest_addr->sin_port = inet->dport;

	return res;
}

int close(int sockfd) {
	struct socket *sock;

	if ((sockfd < 0) || (sockfd >= CONFIG_MAX_KERNEL_SOCKETS)) {
		return -EBADF;
	}

	sock = opened_sockets[sockfd];
	if (sock == NULL) {
		return -EBADF;
	}

	opened_sockets[sockfd] = NULL; /* clear cache */

	return kernel_socket_release(sock);
}
