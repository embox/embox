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
#include <kernel/task/idx.h>
#include <net/socket/socket_desc.h>

extern const struct task_idx_ops task_idx_ops_socket;

static inline struct sock * idx2sock(int idx) {
	struct idx_desc *desc = task_self_idx_get(idx);
	return desc != NULL ? task_idx_desc_data(desc) : NULL;
}

static inline int idx2flags(int idx) {
	struct idx_desc *desc = task_self_idx_get(idx);
	return desc != NULL ? *task_idx_desc_flags_ptr(desc) : 0;
}


/* create */
int socket(int domain, int type, int protocol) {
	int ret, sockfd;
	struct sock *sk;
	struct socket_desc *socket_desc;

	socket_desc = socket_desc_create(domain, type, protocol);
	socket_desc_check_perm(socket_desc, SOCKET_DESC_OPS_SOCKET, NULL);

	ret = ksocket(domain, type, protocol, &sk);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	sockfd = task_self_idx_alloc(&task_idx_ops_socket, sk, &sk->ios);
	if (sockfd < 0) {
		ksocket_close(sk);
		SET_ERRNO(EMFILE);
		return -1;
	}

	return sockfd;
}
/* fcntl */
int bind(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct socket_desc_param param = { addr, &addrlen};
	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_BIND, &param);

	ret = kbind(idx2sock(sockfd), addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* open */
int connect(int sockfd, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct socket_desc_param param = { addr, &addrlen};
	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_CONNECT, &param);

	ret = kconnect(idx2sock(sockfd), addr, addrlen,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* open */
int listen(int sockfd, int backlog) {
	int ret;

	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_LISTEN, NULL);

	ret = klisten(idx2sock(sockfd), backlog);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* create open */
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int ret, new_sockfd;
	struct sock *new_sk;
	struct socket_desc_param param = { addr, addrlen};
	struct socket_desc *sdesc;
	struct socket_desc *accept_sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_ACCEPT, &param);


	ret = kaccept(idx2sock(sockfd), addr, addrlen,
			idx2flags(sockfd), &new_sk);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	accept_sdesc = socket_desc_accept(new_sk);
	socket_desc_check_perm(accept_sdesc, SOCKET_DESC_OPS_ACCEPT, NULL);

	new_sockfd = task_self_idx_alloc(&task_idx_ops_socket, new_sk,
			&new_sk->ios);
	if (new_sockfd < 0) {
		ksocket_close(new_sk);
		SET_ERRNO(EMFILE);
		return -1;
	}

	return new_sockfd;
}
/* write */
ssize_t send(int sockfd, const void *buff, size_t size,
		int flags) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_SEND, NULL);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = ksendmsg(idx2sock(sockfd), &msg, idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}
/* open? write */
ssize_t sendto(int sockfd, const void *buff, size_t size,
		int flags, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct socket_desc *sdesc;
	struct socket_desc_param param = { addr, &addrlen};

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_SEND, &param);

	msg.msg_name = (void *)addr;
	msg.msg_namelen = addrlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = (void *)buff;
	iov.iov_len = size;

	ret = ksendmsg(idx2sock(sockfd), &msg, idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}
/* open? write */
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
	int ret;
	struct msghdr msg_;

	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_SEND, NULL);

	memcpy(&msg_, msg, msg != NULL ? sizeof msg_ : 0);
	msg_.msg_flags = flags;

	ret = ksendmsg(idx2sock(sockfd), msg != NULL ? &msg_ : NULL,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return msg_.msg_iov->iov_len;
}
/* read */
ssize_t recv(int sockfd, void *buff, size_t size, int flags) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_RECV, NULL);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = buff;
	iov.iov_len = size;

	ret = krecvmsg(idx2sock(sockfd), &msg, idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return iov.iov_len;
}
/* open? read */
ssize_t recvfrom(int sockfd, void *buff, size_t size,
		int flags, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;
	struct msghdr msg;
	struct iovec iov;

	struct socket_desc *sdesc;
	struct socket_desc_param param = { addr, addrlen};

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_RECV, &param);

	msg.msg_name = (void *)addr;
	msg.msg_namelen = addrlen != NULL ? *addrlen : 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = flags;

	iov.iov_base = buff;
	iov.iov_len = size;

	ret = krecvmsg(idx2sock(sockfd), &msg, idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	if (addrlen != NULL) {
		*addrlen = msg.msg_namelen;
	}

	return iov.iov_len;
}
/* open? read */
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
	int ret;
	struct msghdr msg_;

	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_RECV, NULL);

	memcpy(&msg_, msg, msg != NULL ? sizeof msg_ : 0);
	msg_.msg_flags = flags;

	ret = krecvmsg(idx2sock(sockfd), msg != NULL ? &msg_ : NULL,
			idx2flags(sockfd));
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	msg->msg_name = msg_.msg_name;
	msg->msg_namelen = msg_.msg_namelen;
	msg->msg_flags = msg_.msg_flags;

	return msg_.msg_iov->iov_len;
}
/* fcntl */
int shutdown(int sockfd, int how) {
	int ret;
	struct socket_desc *sdesc;

	sdesc = socket_desc_get(sockfd);
	socket_desc_check_perm(sdesc, SOCKET_DESC_OPS_SHUTDOWN, NULL);

	ret = kshutdown(idx2sock(sockfd), how);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* fcntl */
int getsockname(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;

	ret = kgetsockname(idx2sock(sockfd), addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* fcntl */
int getpeername(int sockfd, struct sockaddr *addr,
		socklen_t *addrlen) {
	int ret;

	ret = kgetpeername(idx2sock(sockfd), addr, addrlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* fcntl */
int getsockopt(int sockfd, int level, int optname, void *optval,
		socklen_t *optlen) {
	int ret;

	ret = kgetsockopt(idx2sock(sockfd), level, optname, optval,
			optlen);
	if (ret != 0){
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
/* fcntl */
int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen) {
	int ret;

	ret = ksetsockopt(idx2sock(sockfd), level, optname, optval,
			optlen);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
