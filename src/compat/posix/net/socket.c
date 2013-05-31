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

#include <net/ksocket.h>
#include <net/socket.h>
#include <net/sock.h>
#include <sys/socket.h>
#include <util/array.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <net/arp_queue.h>

#include <kernel/thread.h>
#include <kernel/event.h>
#include <kernel/task/io_sync.h>
#include <net/socket_registry.h>

static const struct task_idx_ops task_idx_ops_socket;

static inline struct socket * idx2socket(int idx) {
	struct idx_desc *desc = task_self_idx_get(idx);
	return desc != NULL ? task_idx_desc_data(desc) : NULL;
}

static inline int idx2flags(int idx) {
	struct idx_desc *desc = task_self_idx_get(idx);
	return desc != NULL ? *task_idx_desc_flags_ptr(desc) : 0;
}

int socket(int domain, int type, int protocol) {
	int ret, sockfd;
	struct socket *sock;

	ret = ksocket(domain, type, protocol, &sock);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	sockfd = task_self_idx_alloc(&task_idx_ops_socket, sock);
	if (sockfd < 0) {
		ksocket_close(sock);
		SET_ERRNO(EMFILE);
		return -1;
	}

	assert(sock != NULL);
	sock->desc_data = task_idx_indata(task_self_idx_get(sockfd));
	if (type != SOCK_STREAM) {
		idx_io_enable(sock->desc_data, IDX_IO_WRITING);
	}

	return sockfd;
}

int bind(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kbind(sock, addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int connect(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kconnect(sock, addr, addrlen, idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	idx_io_enable(sock->desc_data, IDX_IO_WRITING);

	return 0;
}

int listen(int sockfd, int backlog) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = klisten(sock, backlog);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int ret, new_sockfd;
	struct socket *sock, *new_sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kaccept(sock, addr, addrlen, idx2flags(sockfd), &new_sock);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	new_sockfd = task_self_idx_alloc(&task_idx_ops_socket, new_sock);
	if (new_sockfd < 0) {
		ksocket_close(new_sock);
		SET_ERRNO(EMFILE);
		return -1;
	}

	assert(new_sock != NULL);
	new_sock->desc_data = task_idx_indata(task_self_idx_get(new_sockfd));
	idx_io_enable(new_sock->desc_data, IDX_IO_WRITING);

	return new_sockfd;
}

static int sendmsg_sock(struct socket *sock,
		const struct msghdr *msg, int flags) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}

	ret = ksendmsg(sock, msg, flags);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

ssize_t send(int sockfd, const void *buff, size_t size,
		int flags) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = sendmsg_sock(idx2socket(sockfd), &msg,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

ssize_t sendto(int sockfd, const void *buff, size_t size,
		int flags, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	msg.msg_name = (void *)addr;
	msg.msg_namelen = addrlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = sendmsg_sock(idx2socket(sockfd), &msg,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
	int ret;
	struct msghdr _msg;

	memcpy(&_msg, msg, sizeof _msg);
	_msg.msg_flags = flags;

	ret = sendmsg_sock(idx2socket(sockfd), &_msg,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return _msg.msg_iov->iov_len;
}

static int recvmsg_sock(struct socket *sock,
		struct msghdr *msg, int flags) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}

	ret = krecvmsg(sock, msg, flags);
	if (ret != 0) {
		return ret;
	}

	softirq_lock();
	{
		if (skb_queue_front(sock->sk->sk_receive_queue) == NULL) {
			idx_io_disable(sock->desc_data, IDX_IO_READING);
		}
	}
	softirq_unlock();

	return 0;
}

ssize_t recv(int sockfd, void *buff, size_t size,
		int flags) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = recvmsg_sock(idx2socket(sockfd), &msg,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

ssize_t recvfrom(int sockfd, void *buff, size_t size,
		int flags, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	msg.msg_name = (void *)addr;
	msg.msg_namelen = addrlen != NULL ? *addrlen : 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = recvmsg_sock(idx2socket(sockfd), &msg,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	if (addrlen != NULL) {
		*addrlen = msg.msg_namelen;
	}

	return iov.iov_len;
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
	int ret;
	struct msghdr _msg;

	memcpy(&_msg, msg, sizeof _msg);
	_msg.msg_flags = flags;

	ret = recvmsg_sock(idx2socket(sockfd), &_msg,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	msg->msg_name = _msg.msg_name;
	msg->msg_namelen = _msg.msg_namelen;
	msg->msg_flags = _msg.msg_flags;

	return _msg.msg_iov->iov_len;
}

int shutdown(int sockfd, int how) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kshutdown(sock, how);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int getsockname(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kgetsockname(sock, addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int getpeername(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kgetpeername(sock, addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int getsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t *optlen) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = kgetsockopt(sock, level, optname, optval, optlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen) {
	int ret;
	struct socket *sock;

	sock = idx2socket(sockfd);
	if (sock == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = ksetsockopt(sock, level, optname, optval, optlen);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

static ssize_t socket_read(struct idx_desc *desc, void *buff,
		size_t size) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	assert(desc != NULL);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = recvmsg_sock((struct socket *)task_idx_desc_data(desc),
			&msg, *task_idx_desc_flags_ptr(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

static ssize_t socket_write(struct idx_desc *desc, const void *buff,
		size_t size) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	assert(desc != NULL);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = sendmsg_sock((struct socket *)task_idx_desc_data(desc),
			&msg, *task_idx_desc_flags_ptr(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}

static int socket_close(struct idx_desc *desc) {
	int ret;

	assert(desc != NULL);

	ret = ksocket_close(task_idx_desc_data(desc));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

static const struct task_idx_ops task_idx_ops_socket = {
	.read = socket_read,
	.write = socket_write,
	.close = socket_close
};

#if 1 /********** TODO remove this ****************/
int check_icmp_err(int sockfd) {
	struct socket *sock = idx2socket(sockfd);
	int err = sock->sk->sk_err;
	sk_clear_pending_error(sock->sk);
	return err;
}

void socket_set_encap_recv(int sockfd, sk_encap_hnd hnd) {
	struct socket *sock = idx2socket(sockfd);
	sock->sk->sk_encap_rcv = hnd;
}
#endif
