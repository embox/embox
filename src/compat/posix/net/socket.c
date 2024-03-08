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
#include <errno.h>
#include <stddef.h>
#include <sys/uio.h>
#include <sys/socket.h>

#include <net/socket/ksocket.h>
#include <net/sock.h>
#include <kernel/task.h>

#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>
#include <net/sock.h>
#include <net/socket/socket_desc.h>

#include <kernel/task/resource/idesc_table.h>
#include <util/log.h>

#include <util/err.h>

#include <util/macro.h>

#include <net/net_namespace.h>

static int get_index(struct sock *sk) {
	struct idesc_table *it;

	it = task_resource_idesc_table(task_self());
	assert(it);

	return idesc_table_add(it, (struct idesc *)sk, 0);
}

#define  socket_idesc_check(sockfd, sk) \
	if (!idesc_index_valid(sockfd) || !index_descriptor_get(sockfd)) {\
		return SET_ERRNO(EBADF);            \
	}                                \
	if (NULL == (sk = idesc_sock_get(sockfd))) { \
		return SET_ERRNO(ENOTSOCK);            \
	} \

/* create */
int socket(int domain, int type, int protocol) {
	int sockfd = 0;

	struct sock *sk;

	sk = ksocket(domain, type, protocol);
	if (ptr2err(sk) != 0) {
		return SET_ERRNO(-ptr2err(sk));
	}

	sockfd = get_index(sk);
	if (sockfd < 0) {
		ksocket_close(sk);
		return SET_ERRNO(EMFILE);
	}
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	assign_net_ns(sk->net_ns, get_net_ns());
#endif
	return sockfd;
}
/* fcntl */
int bind(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (!addr || (addrlen <= 0))
		return SET_ERRNO(EINVAL);

	ret = kbind(sk, addr, addrlen);
	if (ret < 0){
		return SET_ERRNO(-ret);
	}

	return 0;
}

/* open */
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (!addr || (addrlen <= 0))
		return SET_ERRNO(EINVAL);

	ret = kconnect(sk, addr, addrlen, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}

/* open */
int listen(int sockfd, int backlog) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	ret = klisten(sk, backlog);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}
/* create open */
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int ret, new_sockfd;
	struct sock *new_sk;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (((addr == NULL) && (addrlen != NULL))
			|| ((addr != NULL) && (addrlen == NULL))
			|| ((addrlen != NULL) && (*addrlen <= 0))) {
		return SET_ERRNO(EINVAL);
	}

	ret = kaccept(sk, addr, addrlen, sk->idesc.idesc_flags, &new_sk);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	assign_net_ns(new_sk->net_ns, sk->net_ns);
#endif
	new_sockfd = get_index(new_sk);
	if (new_sockfd < 0) {
		ksocket_close(new_sk);
		return SET_ERRNO(-new_sockfd);
	}

	return new_sockfd;
}
/* write */
ssize_t send(int sockfd, const void *buff, size_t size,
		int flags) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (sk->shutdown_flag & (SHUT_WR + 1))
		return SET_ERRNO(EPIPE);

//	if (!size)
//		return 0;

	/* TODO remove this */
	if (flags & (MSG_EOR | MSG_OOB)) {
		log_error("MSG_EOR, MSG_OOB flags are not supported");
		return SET_ERRNO(EOPNOTSUPP);
	}

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = ksendmsg(sk, &msg, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}
	return ret;
}
/* open? write */
ssize_t sendto(int sockfd, const void *buff, size_t size,
		int flags, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (sk->shutdown_flag & (SHUT_WR + 1))
		return SET_ERRNO(EPIPE);

//	if (!size)
//		return 0;

	/* TODO remove this */
	if (flags != 0) {
		log_error("flags are not supported");
		return SET_ERRNO(EOPNOTSUPP);
	}

	msg.msg_name = (void *)addr;
	msg.msg_namelen = addrlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = ksendmsg(sk, &msg, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}
	return ret;
}

/* open? write */
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
	int ret;
	struct msghdr msg_;

	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (sk->shutdown_flag & (SHUT_WR + 1))
		return SET_ERRNO(EPIPE);

	/* TODO remove this */
	if (flags != 0) {
		log_error("flags are not supported");
		return SET_ERRNO(EOPNOTSUPP);
	}

	if ((msg == NULL) || (msg->msg_iov == NULL)
			|| (msg->msg_iovlen == 0)) {
		return SET_ERRNO(EINVAL);
	}
	memcpy(&msg_, msg, sizeof (msg_));
	msg_.msg_flags = flags;

	ret = ksendmsg(sk, &msg_, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}
	return ret;
}

/* read */
ssize_t recv(int sockfd, void *buff, size_t size, int flags) {
	int ret;
	struct msghdr msg;
	struct iovec iov;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		return SET_ERRNO(EPIPE);
	}

#if 0
	/* TODO remove this */
	if (flags != 0) {
		log_error("flags are not supported");
		return SET_ERRNO(EOPNOTSUPP);
	}
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = buff;
	iov.iov_len = size;

	ret = krecvmsg(sk, &msg, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}

/* open? read */
ssize_t recvfrom(int sockfd, void *buff, size_t size,
		int flags, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (sk->shutdown_flag & (SHUT_RD + 1)) {
		return SET_ERRNO(EPIPE);
	}

#if 0
	/* TODO remove this */
	if (flags != 0) {
		log_error("flags are not supported");
		return SET_ERRNO(EOPNOTSUPP);
	}
#endif

	msg.msg_name = (void *)addr;
	msg.msg_namelen = addrlen != NULL ? *addrlen : 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = buff;
	iov.iov_len = size;

	ret = krecvmsg(sk, &msg, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	if (addrlen != NULL) {
		*addrlen = msg.msg_namelen;
	}

	return ret;
}

/* open? read */
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
	int ret;
	struct msghdr msg_;

	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (sk->shutdown_flag & (SHUT_RD + 1))
		return SET_ERRNO(EPIPE);

	if ((msg == NULL) || (msg->msg_iov == NULL)
			|| (msg->msg_iovlen == 0)) {
		return SET_ERRNO(EINVAL);
	}

	/* TODO remove this */
	if (flags != 0) {
		log_error("flags are not supported");
		return SET_ERRNO(EOPNOTSUPP);
	}

	memcpy(&msg_, msg, sizeof msg_);
	msg_.msg_flags = flags;

	ret = krecvmsg(sk, &msg_, sk->idesc.idesc_flags);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	msg->msg_name = msg_.msg_name;
	msg->msg_namelen = msg_.msg_namelen;
	msg->msg_flags = msg_.msg_flags;

	return ret;
}

/* fcntl */
int shutdown(int sockfd, int how) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if (how != SHUT_RD && how != SHUT_WR && how != SHUT_RDWR)
		return SET_ERRNO(EINVAL);

	ret = kshutdown(sk, how);
	if (ret < 0){
		return SET_ERRNO(-ret);
	}

	return 0;
}
/* fcntl */
int getsockname(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return SET_ERRNO(EINVAL);
	}

	ret = kgetsockname(sk, addr, addrlen);
	if (ret < 0){
		return SET_ERRNO(-ret);
	}

	return 0;
}

/* fcntl */
int getpeername(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return SET_ERRNO(EINVAL);
	}

	ret = kgetpeername(sk, addr, addrlen);
	if (ret < 0){
		return SET_ERRNO(-ret);
	}

	return 0;
}
/* fcntl */
int getsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t *optlen) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if ((optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return SET_ERRNO(EINVAL);
	}

	ret = kgetsockopt(sk, level, optname, optval, optlen);
	if (ret < 0){
		return SET_ERRNO(-ret);
	}

	return 0;
}
/* fcntl */
int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen) {
	int ret;
	struct sock *sk;

	socket_idesc_check(sockfd, sk);

	if ((optval == NULL) || (optlen < 0)) {
		return SET_ERRNO(EINVAL);
	}

	ret = ksetsockopt(sk, level, optname, optval, optlen);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}
