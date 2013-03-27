/**
 * @file
 * @brief An implementation of the SOCKET network access protocol.
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <sys/uio.h>

#include <net/inet_sock.h>
#include <net/kernel_socket.h>
#include <net/socket.h>
#include <net/sock.h>
#include <sys/socket.h>
#include <util/array.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <net/arp_queue.h>

#include <kernel/thread.h>
#include <kernel/thread/event.h>
#include <kernel/task/io_sync.h>
#include <net/socket_registry.h>

static ssize_t this_read(struct idx_desc *socket, void *buf, size_t nbyte);
static ssize_t this_write(struct idx_desc *socket, const void *buf, size_t nbyte);
static int this_ioctl(struct idx_desc *socket, int request, void *);
static int this_close(struct idx_desc *socket);

const struct task_idx_ops task_idx_ops_socket = {
	.read = this_read,
	.write = this_write,
	.close = this_close,
	.ioctl = this_ioctl
};

static struct socket *idx2sock(int fd) {
	return (struct socket *) task_idx_desc_data(task_self_idx_get(fd));
}

int socket(int domain, int type, int protocol) {
	int ret, res;
	struct socket *sock;

	ret = kernel_socket_create(domain, type, protocol, &sock, NULL, NULL);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	res = task_self_idx_alloc(&task_idx_ops_socket, sock);
	if (res < 0) {
		kernel_socket_release(sock);
		SET_ERRNO(EMFILE);
		return -1;
	}

	sock->desc = task_self_idx_get(res);

	assert(sock->state != SS_CONNECTED); /* XXX ?? */

	/**
	 * Block stream socket on writing while
	 * it is unconnected. Otherwise unblock it.
	 */
	if (type != SOCK_STREAM) {
		idx_io_enable(task_idx_indata(sock->desc), IDX_IO_WRITING);
	}

	return res;
}

int connect(int sockfd, const struct sockaddr *daddr, socklen_t daddrlen) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_connect(sock, daddr, daddrlen, 0);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	/* If connection established, than we can write in this socket always. */
	idx_io_enable(task_idx_indata(sock->desc), IDX_IO_WRITING);

	return 0;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_bind(sock, addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int listen(int sockfd, int backlog) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_listen(sock, backlog);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int ret, res;
	struct socket *sock, *new_sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_accept(sock, &new_sock, addr, addrlen);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	res = task_self_idx_alloc(&task_idx_ops_socket, new_sock);
	if (res < 0) {
		kernel_socket_release(new_sock);
		SET_ERRNO(EMFILE);
		return -1;
	}

	new_sock->desc = task_self_idx_get(res);

	/**
	 * If connection established, than we can
	 * write in this socket always.
	 */
	idx_io_enable(task_idx_indata(new_sock->desc), IDX_IO_WRITING);

	return res;
}

static ssize_t sendto_sock(struct socket *sock, const void *buf, size_t len,
		int flags, const struct sockaddr *daddr, socklen_t daddrlen) {
	int ret;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	iov.iov_base = (void *)buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	assert(sock->sk);

	switch (sock->type) {
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_PACKET:
		if (daddr == NULL) {
			SET_ERRNO(EDESTADDRREQ);
			return -1;
		}
		if (daddrlen != sizeof *dest_addr) {
			SET_ERRNO(EINVAL);
			return -1;
		}
		dest_addr = (struct sockaddr_in *)daddr;
		inet = inet_sk(sock->sk);
		inet->daddr = dest_addr->sin_addr.s_addr;
		inet->dport = dest_addr->sin_port;
		break;
	}

	if (sock->sk->sk_shutdown & (SHUT_WR + 1)) {
		SET_ERRNO(EPIPE);
		return -1;
	}

	/* socket is ready for usage and has no data transmitting errors yet */
	sock->sk->sk_err = -1; /* XXX ?? */
#if 0
	sock_set_ready(sock->sk);
#endif

	ret = kernel_socket_sendmsg(NULL, sock, &m, len);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return (ssize_t)iov.iov_len;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *daddr, socklen_t daddrlen) {
	return sendto_sock(idx2sock(sockfd), buf, len, flags, daddr, daddrlen);
}

static ssize_t recvfrom_sock(struct socket *sock, void *buf, size_t len,
		int flags, struct sockaddr *daddr, socklen_t *daddrlen) {
	int ret;
	struct inet_sock *inet;
	struct iovec iov;
	struct msghdr m;
	struct sockaddr_in *dest_addr;

	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	iov.iov_base = buf;
	iov.iov_len = len;
	m.msg_iov = &iov;

	ret = kernel_socket_recvmsg(NULL, sock, &m, len, flags);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	if ((daddr != NULL) && (daddrlen != NULL)) {
		inet = inet_sk(sock->sk);
		dest_addr = (struct sockaddr_in *)daddr;
		dest_addr->sin_family = AF_INET;
		dest_addr->sin_addr.s_addr = inet->daddr;
		dest_addr->sin_port = inet->dport;
		memset(&dest_addr->sin_zero[0], 0, sizeof dest_addr->sin_zero);
		*daddrlen = sizeof *dest_addr;
	}

	return (ssize_t)iov.iov_len;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
			struct sockaddr *daddr, socklen_t *daddrlen) {
	ssize_t ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	sched_lock();
	{
		ret = recvfrom_sock(sock, buf, len, flags, daddr, daddrlen);
		/* if !O_NONBLOCK on socket's file descriptor {*/
		if (ret == -1) {
			event_wait_ms(&sock->sk->sock_is_not_empty, SCHED_TIMEOUT_INFINITE);
			ret = recvfrom_sock(sock, buf, len, flags, daddr, daddrlen);
		}
		/* } */
	}
	sched_unlock();

	return ret;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
	return recvfrom(sockfd, buf, len, flags, NULL, NULL);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!sk_is_connected(sock)) {
		SET_ERRNO(ENOTCONN);
		return -1;
	}

	return sendto_sock(sock, buf, len, flags, NULL, 0);
}

int shutdown(int sockfd, int how) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	sock->sk->sk_shutdown |= (how + 1);

	ret = kernel_socket_shutdown(sock, how);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

#if 0
int socket_close(int sockfd) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_release(sock);
	if (ret < 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
#endif

static ssize_t this_read(struct idx_desc *data, void *buf, size_t nbyte) {
	struct socket *sock = task_idx_desc_data(data);
	ssize_t len;

	len = recvfrom_sock(task_idx_desc_data(data), buf, nbyte, * task_idx_desc_flags_ptr(data), NULL, 0);

	softirq_lock();
	{
		if (skb_queue_front(sock->sk->sk_receive_queue) == NULL) {
			idx_io_disable(task_idx_indata(data), IDX_IO_READING);
		}
	}
	softirq_unlock();

	return len;
}

static ssize_t this_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	return sendto_sock(task_idx_desc_data(data), buf, nbyte, 0, NULL, 0);
}

static int this_ioctl(struct idx_desc *socket, int request, void *data) {
	return 0;
}

static int this_close(struct idx_desc *socket) {
	int ret;

	ret = kernel_socket_release(task_idx_desc_data(socket));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int getsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t *optlen) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_getsockopt(sock, level, optname, optval, optlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int setsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t optlen) {
	int ret;
	struct socket *sock;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kernel_socket_setsockopt(sock, level, optname, optval, optlen);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int getsockname(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	struct socket *sock;
	struct inet_sock *inet;
	struct sockaddr_in *src_addr;

	sock = idx2sock(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	assert(sock->sk);

	switch (sock->sk->sk_type) {
	default:
		// FIXME: not possible to extract socket family from descriptor
//	case AF_INET:
		if (addr == NULL) {
			SET_ERRNO(EBADF);
			return -1;
		}
		if (*addrlen < sizeof *src_addr) {
			SET_ERRNO(EINVAL);
			return -1;
		}
		src_addr = (struct sockaddr_in *)addr;
		inet = inet_sk(sock->sk);
		src_addr->sin_family = AF_INET;
		src_addr->sin_addr.s_addr = inet->rcv_saddr;
		src_addr->sin_port = inet->sport;
		memset(&src_addr->sin_zero[0], 0, sizeof src_addr->sin_zero);
		*addrlen = sizeof *src_addr;
		break;
//	default:
//		SET_ERRNO(EINVAL);
//		return -1;
	}

	return 0;
}


#if 1 /********** TODO remove this ****************/
int check_icmp_err(int sockfd) {
	struct socket *sock = idx2sock(sockfd);
	int err = sock->sk->sk_err;
	sk_clear_pending_error(sock->sk);
	return err;
}

void socket_set_encap_recv(int sockfd, sk_encap_hnd hnd) {
	struct socket *sock = idx2sock(sockfd);
	sock->sk->sk_encap_rcv = hnd;
}
#endif
