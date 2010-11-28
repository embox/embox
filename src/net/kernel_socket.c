/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.2010
 * @author Anton Bondarev
 */
#include <errno.h>
#include <kernel/irq.h>
#include <lib/list.h>
#include <net/in.h>
#include <net/net.h>
#include <asm/system.h>
#include <linux/init.h>

typedef struct socket_info {
	/*it must be first member! We use casting in sock_realize function*/
	struct socket    sock;
	int              sockfd;
	struct list_head list __attribute__ ((aligned (4)));
} socket_info_t __attribute__ ((aligned (4)));

static socket_info_t sockets_pull[CONFIG_MAX_KERNEL_SOCKETS];
static LIST_HEAD(head_free_sk);

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
	struct list_head *entry;
	unsigned long flags;

	local_irq_save(flags);

	if (list_empty(&head_free_sk)) {
		local_irq_restore(flags);
		return NULL;
	}
	entry = (&head_free_sk)->next;
	list_del_init(entry);
	sock = (struct socket *) list_entry(entry, socket_info_t, list);

	local_irq_restore(flags);

	return sock;
}

int kernel_sock_release(struct socket *sock) {
	socket_info_t *sock_info;
	int ret;
	unsigned long irq_old;
	if ((NULL == sock) || (NULL == sock->ops)
			|| (NULL == sock->ops->release)) {
		return -1;
	}
	/*release struct sock*/
	ret = sock->ops->release(sock);

	local_irq_save(irq_old);
	/* return sock into pull
	 * we can cast like this because struct socket is first element of
	 * struct socket_info
	 */
	sock_info = (socket_info_t *) sock;
	list_add(&sock_info->list, &head_free_sk);
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

int __init kernel_sock_init(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(sockets_pull); i++) {
		list_add(&(&sockets_pull[i])->list, &head_free_sk);
		(&sockets_pull[i])->sockfd = i;
	}
	return 0;
}

int sock_create_kern(int family, int type, int protocol, struct socket **res) {
	return __sock_create(family, type, protocol, res, 1);
}

int kernel_bind(struct socket *sock, const struct sockaddr *addr,
			socklen_t addrlen) {
	return sock->ops->bind(sock, (struct sockaddr *)addr, addrlen);
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
	return sock->ops->connect(sock, (struct sockaddr *)addr, addrlen, flags);
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
	if (fd < 0 || fd >= ARRAY_SIZE(sockets_pull)) {
		return NULL;
	}
	return &(&sockets_pull[fd])->sock;
}

int sock_get_fd(struct socket *sock) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(sockets_pull); i++) {
		if(&(&sockets_pull[i])->sock == sock) {
			return (&sockets_pull[i])->sockfd;
		}
	}
	return -1;
}

int sock_register(const struct net_proto_family *ops) {
	int err;
	if (ops->family >= NPROTO) {
		LOG_ERROR("protocol %d >= NPROTO(%d)\n", ops->family,
				NPROTO);
		return -ENOBUFS;
	}

	if (net_families[ops->family]) {
		err = -EEXIST;
	} else {
		net_families[ops->family] = ops;
		err = 0;
	}

	TRACE("NET: Registered protocol family %d\n", ops->family);
	return err;
}

void sock_unregister(int family) {
	if(family < 0 || family >= NPROTO) {
		return;
	}

	net_families[family] = NULL;
	TRACE("NET: Unregistered protocol family %d\n", family);
}
