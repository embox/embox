/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.10
 * @author Anton Bondarev
 */

#include <errno.h>
#include <kernel/irq.h>
#include <net/in.h>
#include <net/net.h>
#include <asm/system.h>
#include <linux/init.h>
#include <util/array.h>
#include <mem/misc/pool.h>

typedef struct socket_info {
	/*it must be first member! We use casting in sock_realize function*/
	struct socket    sock;
	int              sockfd;
} socket_info_t __attribute__ ((aligned (4)));

/* pool for allocate sockets */
POOL_DEF(sockets_pool, socket_info_t, CONFIG_MAX_KERNEL_SOCKETS);

/**
 * The protocol list. Each protocol is registered in here.
 */
static const struct net_proto_family *net_families[NPROTO];

/**
 * Allocate a socket.
 * Now we only allocate memory for structure of socket (struct socket).
 *
 * In Linux it must use socketfs and they use inodes for it.
 */
static struct socket *sock_alloc(void) {
	struct socket *sock;
	socket_info_t *sock_info;
	unsigned long flags;

	local_irq_save(flags);

	if ((sock_info = pool_alloc(&sockets_pool)) == NULL) {
		local_irq_save(flags);
		return NULL;
	}

	sock_info->sockfd = sock_info - (socket_info_t *)sockets_pool.storage;
	sock = (struct socket *)sock_info;
	local_irq_restore(flags);

	return sock;
}

int kernel_sock_release(struct socket *sock) {
	int ret;
	unsigned long irq_old;
	if ((NULL == sock) || (NULL == sock->ops)
			|| (NULL == sock->ops->release)) {
		return -1;
	}
	/* release struct sock */
	ret = sock->ops->release(sock);

	local_irq_save(irq_old);
	/* return sock into pool */
	pool_free(&sockets_pool, sock);
	local_irq_restore(irq_old);
	return ret;
}

static int __sock_create(int family, int type, int protocol,
		struct socket **res, int kern) {
	int err;
	struct socket *sock;
	const struct net_proto_family *pf;

	/*
	 * Check protocol is in range
	 */
	if (family < 0 || family >= NPROTO)
		return -EINVAL;
	if (type < 0 || type >= SOCK_MAX)
		return -ENFILE;

	if (family == PF_INET && type == SOCK_PACKET) {
		family = PF_PACKET;
	}
	/*pf = rcu_dereference(net_families[family]);*/
	pf = (const struct net_proto_family *) net_families[family];
	if (NULL == pf || NULL == pf->create) {
		return -EINVAL;
	}
	/*
	 here must be code for trying socket (permition and so on)
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
	if (!sock) {
		return -ENOMEM;
	}

	sock->type = type;

	err = pf->create(sock, protocol);
	if (err < 0) {
		kernel_sock_release(sock);
		return err;
	}
	/*here we must be code for trying socket (permition and so on)
	 err = security_socket_post_create(sock, family, type, protocol, kern);
	 */
	*res = sock;
	return 0;
}

int sock_create_kern(int family, int type, int protocol, struct socket **res) {
	return __sock_create(family, type, protocol, res, 1);
}

int kernel_bind(struct socket *sock, const struct sockaddr *addr,
			socklen_t addrlen) {
	return sock->ops->bind(sock, (struct sockaddr *) addr, addrlen);
}

int kernel_listen(struct socket *sock, int backlog) {
	return sock->ops->listen(sock, backlog);
}

int kernel_accept(struct socket *sock, struct socket **newsock, int flags) {
	int err;
	err = sock->ops->accept(sock, *newsock, flags);
	if (err < 0) {
		kernel_sock_release(*newsock);
	}
	(*newsock)->ops = sock->ops;
	return err;
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

struct socket *sockfd_lookup(int fd) {
	if (fd < 0 || fd >= CONFIG_MAX_KERNEL_SOCKETS) {
		return NULL;
	}
	socket_info_t *socket_info = (socket_info_t *)sockets_pool.storage + fd;
	return (struct socket *) socket_info ;
}

int sock_get_fd(struct socket *sock) {
	return ((socket_info_t *) sock)->sockfd;
}

int sock_register(const struct net_proto_family *ops) {
	int err;
	if (ops->family >= NPROTO) {
		return -ENOBUFS;
	}
	if (net_families[ops->family]) {
		err = -EEXIST;
	} else {
		net_families[ops->family] = ops;
		err = 0;
	}

	return err;
}

void sock_unregister(int family) {
	if (family < 0 || family >= NPROTO) {
		return;
	}

	net_families[family] = NULL;
}
