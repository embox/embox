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
#include <util/array.h>
#include <kernel/task.h>
#include <net/port.h>

#ifndef CONFIG_MAX_KERNEL_SOCKETS
#define CONFIG_MAX_KERNEL_SOCKETS 0x4
#endif
/* TODO: remove all below from here */

extern const struct task_res_ops * __task_res_ops[];

static ssize_t this_read(int fd, const void *buf, size_t nbyte) {
	return recvfrom(fd, (void *) buf, nbyte, 0, NULL, 0);
}

static ssize_t this_write(int fd, const void *buf, size_t nbyte) {
	return sendto(fd, buf, nbyte, 0, NULL, 0);
}

static struct task_res_ops ops = {
	.type = TASK_IDX_TYPE_SOCKET,
	.read = this_read,
	.write = this_write,
	.close = socket_close
};

ARRAY_SPREAD_ADD(__task_res_ops, &ops);

static struct socket *idx2sock(int fd) {
	return task_self_idx_get(fd)->socket;
}

int socket(int domain, int type, int protocol) {
	int res;
	struct socket *sock;

	res = kernel_socket_create(domain, type, protocol, &sock);
	if (res < 0) {
		return res; /* return error code */
	}

	return task_res_idx_alloc(task_self_res(), TASK_IDX_TYPE_SOCKET, sock);
}

int connect(int sockfd, const struct sockaddr *daddr, socklen_t daddrlen) {
	struct socket *sock;

	if (sockfd < 0) {
		return -EBADF;
	}

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_socket_connect(sock, daddr, daddrlen, 0);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	struct socket *sock;

	if (sockfd < 0) {
		return -EBADF;
	}

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_socket_bind(sock, addr, addrlen);
}

int listen(int sockfd, int backlog) {
	struct socket *sock;

	if (sockfd < 0) {
		return -EBADF;
	}
	sock = idx2sock(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}
	return kernel_socket_listen(sock, backlog);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	struct socket *sock;
	int res;

	if (sockfd < 0) {
		return -EBADF;
	}
	sock = idx2sock(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	if ((res = kernel_socket_accept(sock, addr, addrlen)) < 0) {

	}

	return sockfd;
}

static size_t sendto_sock(struct socket *sock, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {
	int res;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

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

	socket_set_port_type(sock);
	if (inet->sport == 0) {
		//inet->sport = 666;
		inet->sport = socket_get_free_port(inet->sport_type);
	}

	res = kernel_socket_sendmsg(NULL, sock, &m, len);
	if (res < 0) {
		return (ssize_t)res;
	}

	return (ssize_t)len;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {

	return sendto_sock(idx2sock(sockfd), buf, len, flags, daddr, daddrlen);

}

static ssize_t recvfrom_sock(struct socket *sock, void *buf, size_t len, int flags,
			struct sockaddr *daddr, socklen_t *daddrlen) {
	int res;
//	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
//	struct sockaddr_in *dest_addr;

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

#if 0
	/* FIXME: Error:
	 * sizeof(struct sock) < sizeof(struct inet_sock), so reinterpret
	 * sock->sk as pointer to struct inet_sock pointer is incorrect,
	 * because after that we will use memory of the next field in
	 * struct socket i.e. const struct proto_ops *ops; --Ilia Vaprol
	 */
	inet = inet_sk(sock->sk);
	dest_addr = (struct sockaddr_in *)daddr;
	dest_addr->sin_addr.s_addr = inet->daddr;
	dest_addr->sin_port = inet->dport;
#endif

	return res;
}


ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
			struct sockaddr *daddr, socklen_t *daddrlen) {

	return recvfrom_sock(idx2sock(sockfd), buf, len, flags, daddr, daddrlen);
}

int socket_close(int sockfd) {
	struct socket *sock;

	if (sockfd < 0) {
		return -EBADF;
	}

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		return -EBADF;
	}

	return kernel_socket_release(sock);
}
