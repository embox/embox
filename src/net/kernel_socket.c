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
#include <mem/misc/pool.h>
#include <net/net.h>
#include <net/socket.h>
#include <stddef.h>
#include <types.h>

struct socket_info {
	/* it must be first member! We use casting in sock_realize function*/
	struct socket    sock;
	int              sockfd;
};

/* pool for allocate sockets */
POOL_DEF(sockets_pool, struct socket_info, CONFIG_MAX_KERNEL_SOCKETS);

/**
 * The protocol list. Each protocol is registered in here.
 */
static const struct net_proto_family *net_families[NPROTO] = {0};

/**
 * Allocate a socket.
 * Now we only allocate memory for structure of socket (struct socket).
 *
 * In Linux it must use socketfs and they use inodes for it.
 */
static struct socket * sock_alloc(void) {
	struct socket_info *sock_info;
	ipl_t flags;

	flags = ipl_save();

	sock_info = pool_alloc(&sockets_pool);
	if (sock_info == NULL) {
		ipl_restore(flags);
		return NULL;
	}

	ipl_restore(flags);

	sock_info->sockfd = sock_info - (struct socket_info *)sockets_pool.storage;

	return (struct socket *)sock_info;
}

int kernel_sock_release(struct socket *sock) {
	int res;
	ipl_t flags;

	if ((sock != NULL) && (sock->ops != NULL)
			&& (sock->ops->release != NULL)) {
		/* release struct sock */
		res = sock->ops->release(sock);
		if (res < 0) {
			return res;
		}
	}

	flags = ipl_save();
	/* return sock into pool */
	pool_free(&sockets_pool, sock);
	ipl_restore(flags);

	return ENOERR;
}

static int __sock_create(int family, int type, int protocol,
		struct socket **psock) {
	int res;
	struct socket *sock;
	const struct net_proto_family *pf;

	/*
	 * Check protocol is in range
	 */
	if ((family < 0) || (family >= NPROTO)) {
		return -EINVAL;
	}
	if ((type < 0) || (type >= SOCK_MAX)) {
		return -EINVAL;
	}

	if ((family == PF_INET) && (type == SOCK_PACKET)) {
		family = PF_PACKET;
	}

	/*pf = rcu_dereference(net_families[family]);*/

	pf = net_families[family];
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
	sock = sock_alloc();
	if (sock == NULL) {
		return -ENOMEM;
	}

	sock->type = type;

	res = pf->create(sock, protocol);
	if (res < 0) {
		kernel_sock_release(sock);
		return res;
	}

	/* TODO here we must be code for trying socket (permition and so on)
	 err = security_socket_post_create(sock, family, type, protocol, kern);
	 */

	*psock = sock;

	return ENOERR;
}

int kernel_sock_create(int family, int type, int protocol, struct socket **res) {
	return __sock_create(family, type, protocol, res);
}

int kernel_bind(struct socket *sock, const struct sockaddr *addr,
			socklen_t addrlen) {
	return sock->ops->bind(sock, (struct sockaddr *) addr, addrlen);
}

int kernel_listen(struct socket *sock, int backlog) {
	return sock->ops->listen(sock, backlog);
}

int kernel_accept(struct socket *sock, struct socket **newsock, int flags) {
	int res;

	res = sock->ops->accept(sock, *newsock, flags);
	if (res < 0) {
		kernel_sock_release(*newsock);
	}

	(*newsock)->ops = sock->ops;

	return res;
}

int kernel_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	return sock->ops->connect(sock, (struct sockaddr *) addr, addrlen, flags);
}

int kernel_getsockname(struct socket *sock,
			struct sockaddr *addr, int *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 0);
}

int kernel_getpeername(struct socket *sock,
			struct sockaddr *addr, int *addrlen) {
	return sock->ops->getname(sock, addr, addrlen, 1);
}

int kernel_getsockopt(struct socket *sock, int level, int optname,
		char *optval, int *optlen) {
	return sock->ops->getsockopt(sock, level, optname, optval, optlen);
}

int kernel_setsockopt(struct socket *sock, int level, int optname,
		char *optval, int optlen) {
	return sock->ops->setsockopt(sock, level, optname, optval, optlen);
}

int kernel_sendmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len) {
	return sock->ops->sendmsg(iocb, sock, m, total_len);
}

int kernel_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len, int flags) {
	return sock->ops->recvmsg(iocb, sock, m, total_len, flags);
}

#if 0
int kernel_sendpage(struct socket *sock, struct page *page, int offset,
		size_t size, int flags) {
	if (sock->ops->sendpage)
		return sock->ops->sendpage(sock, page, offset, size, flags);

	return sock_no_sendpage(sock, page, offset, size, flags);
}

int kernel_sock_ioctl(struct socket *sock, int cmd, unsigned long arg) {
	mm_segment_t oldfs = get_fs();
	int err;

	set_fs(KERNEL_DS);
	err = sock->ops->ioctl(sock, cmd, arg);
	set_fs(oldfs);

	return err;
}
#endif

struct socket * sockfd_lookup(int fd) {
	struct socket_info *socket_info;

	if ((fd < 0) || (fd >= CONFIG_MAX_KERNEL_SOCKETS)) {
		return NULL;
	}

	socket_info = (struct socket_info *)sockets_pool.storage + fd;

	return (struct socket *)socket_info ;
}

int sock_get_fd(struct socket *sock) {
	return ((struct socket_info *)sock)->sockfd;
}

int sock_register(const struct net_proto_family *ops) {
	if (ops->family >= NPROTO) {
		return -ENOBUFS; /* FIXME mb -EINVAL ? */
	}
	if (net_families[ops->family] != NULL) {
		return -EEXIST;
	}

	net_families[ops->family] = ops;

	return ENOERR;
}

void sock_unregister(int family) {
	if ((family < 0) || (family >= NPROTO)) {
		return;
	}

	net_families[family] = NULL;
}
