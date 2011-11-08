/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <hal/ipl.h>
#include <linux/aio.h>
#include <net/net.h>
#include <net/socket.h>
#include <stddef.h>
#include <types.h>


int kernel_socket_create(int family, int type, int protocol, struct socket **psock) {
	int res;
	struct socket *sock;
	const struct net_proto_family *pf;

	if ((type < 0) || (type >= SOCK_MAX)) {
		return -EINVAL;
	}
	if ((family == PF_INET) && (type == SOCK_PACKET)) {
		family = PF_PACKET;
	}

	pf = socket_repo_get_family(family);

	if ((pf == NULL) || (pf->create == NULL)) {
		return -EINVAL;
	}

	/* TODO here must be code for trying socket (permition and so on)
	 err = security_socket_create(family, type, protocol, kern);
	 if (err)
	 return err;
	 */

	/*
	 * Allocate the socket and allow the family to set things up. if
	 * the protocol is 0, the family is instructed to select an appropriate
	 * default.
	 */
	sock = socket_alloc();
	if (sock == NULL) {
		return -ENOMEM;
	}

	sock->type = type;

	res = pf->create(sock, protocol);
	if (res < 0) {
		socket_free(sock);
		return res;
	}

	/* TODO here we must be code for trying socket (permition and so on)
	 err = security_socket_post_create(sock, family, type, protocol, kern);
	 */

	//res = sock - (struct socket *)socket_pool.storage; /* calculate sockfd */
	res = task_idx_alloc(TASK_IDX_TYPE_SOCKET);
	*psock = sock; /* and save struct */

	return res;
}

int kernel_socket_release(struct socket *sock) {
	int res;

	if ((sock != NULL) && (sock->ops != NULL)
			&& (sock->ops->release != NULL)) {
		res = sock->ops->release(sock); /* release struct sock */
		if (res < 0) {
			return res;
		}
	}

	socket_free(sock);

	return ENOERR;
}

int kernel_socket_bind(struct socket *sock, const struct sockaddr *addr,
			socklen_t addrlen) {
	return sock->ops->bind(sock, (struct sockaddr *) addr, addrlen);
}

int kernel_socket_listen(struct socket *sock, int backlog) {
	return sock->ops->listen(sock, backlog);
}

int kernel_socket_accept(struct socket *sock, struct socket **newsock, int flags) {
	int res;

	res = sock->ops->accept(sock, *newsock, flags);
	if (res < 0) {
		return res;
#if 0
		kernel_socket_release(*newsock); /* FIXME must be free in accept() function */
#endif
	}

	(*newsock)->ops = sock->ops;

	return res;
}

int kernel_socket_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	return sock->ops->connect(sock, (struct sockaddr *) addr, addrlen, flags);
}

int kernel_socket_getsockname(struct socket *sock,
			struct sockaddr *addr, int *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 0);
}

int kernel_socket_getpeername(struct socket *sock,
			struct sockaddr *addr, int *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 1);
}

int kernel_socket_getsockopt(struct socket *sock, int level, int optname,
		char *optval, int *optlen) {
	return sock->ops->getsockopt(sock, level, optname, optval, optlen);
}

int kernel_socket_setsockopt(struct socket *sock, int level, int optname,
		char *optval, int optlen) {
	return sock->ops->setsockopt(sock, level, optname, optval, optlen);
}

int kernel_socket_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len) {
	return sock->ops->sendmsg(iocb, sock, m, total_len);
}

int kernel_socket_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len, int flags) {
	return sock->ops->recvmsg(iocb, sock, m, total_len, flags);
}

