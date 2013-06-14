/**
 * @file
 * @brief Implements socket interface function for kernel mode.
 *
 * @date 13.01.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <net/socket.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <net/netdevice.h>

#include <util/math.h>

#include <net/sock.h>
#include <util/sys_log.h>
#include <net/ksocket.h>
#include <net/socket_registry.h>
#include <embox/net/family.h>

#include <framework/mod/options.h>
#include <hal/ipl.h>
#include <mem/misc/pool.h>

#define MODOPS_AMOUNT_SOCKET OPTION_GET(NUMBER, amount_socket)

POOL_DEF(socket_pool, struct socket, MODOPS_AMOUNT_SOCKET);

static struct socket * socket_alloc(void) {
	ipl_t sp;
	struct socket *sock;

	sp = ipl_save();
	{
		sock = pool_alloc(&socket_pool);
	}
	ipl_restore(sp);

	return sock;
}

static void socket_free(struct socket *sock) {
	ipl_t sp;

	sp = ipl_save();
	{
		pool_free(&socket_pool, sock);
	}
	ipl_restore(sp);
}

static int ksocket_ext(int family, int type, int protocol,
		struct sock *sk, struct socket **out_sock) {
	int ret;
	struct socket *new_sock;

	assert(out_sock != NULL);

	new_sock = socket_alloc();
	if (new_sock == NULL) {
		return -ENOMEM;
	}

	if (sk == NULL) {
		ret = sock_create(family, type, protocol, &sk);
		if (ret != 0) {
			socket_free(new_sock);
			return ret;
		}
	}

	sk->sk_socket = new_sock;

	new_sock->sk = sk;
	new_sock->desc_data = NULL;
	new_sock->socket_node = NULL;

	ret = sr_add_socket_to_registry(new_sock);
	if (ret != 0) {
		socket_free(new_sock);
		sock_close(sk);
		return ret;
	}
	sk_set_connection_state(new_sock, UNCONNECTED);

	*out_sock = new_sock;

	return 0;
}

int ksocket(int family, int type, int protocol,
		struct socket **out_sock) {
	if (out_sock == NULL) {
		return -EINVAL;
	}

	return ksocket_ext(family, type, protocol, NULL, out_sock);
}

int ksocket_close(struct socket *sock) {
	if (sock == NULL) {
		return -EBADF;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	sk_set_connection_state(sock, DISCONNECTING);

	sr_remove_saddr(sock);

	if (0 != sr_remove_socket_from_registry(sock)) {
		LOG_WARN("ksocket_close",
				"couldn't remove entry from registry");
	}

	if (0 != sock_close(sock->sk)) {
		LOG_WARN("ksocket_close", "can't close socket");
	}

	socket_free(sock);

	return 0;
}

int kbind(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (sk_is_bound(sock)) {
		return -EINVAL;
	}
	else if (sock->sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if (!sr_is_saddr_free(sock, (struct sockaddr *)addr)) {
		return -EADDRINUSE;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->bind == NULL) {
		return -EOPNOTSUPP;
	}

	ret = sock->sk->f_ops->bind(sock->sk, addr, addrlen);
	if (ret != 0) {
		return ret;
	}

	sk_set_connection_state(sock, BOUND);
	sr_set_saddr(sock, addr);

	return 0;
}

int kconnect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (sock->sk->opt.so_domain != addr->sa_family) {
		return -EAFNOSUPPORT;
	}
	else if ((sock->sk->opt.so_type == SOCK_STREAM)
			&& sk_is_connected(sock)) {
		return -EISCONN;
	}
	else if (sk_is_listening(sock)) {
		return -EOPNOTSUPP;
	}
	else if (sk_get_connection_state(sock) == CONNECTING) {
		return -EALREADY;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->connect == NULL) {
		return -ENOSYS;
	}

	if (!sk_is_bound(sock)) {
		/* FIXME */
		if (sock->sk->f_ops->bind_local == NULL) {
			return -EINVAL;
		}
		ret = sock->sk->f_ops->bind_local(sock->sk);
		if (ret != 0) {
			return ret;
		}
		/* sk_set_connection_state(sock, BOUND); */
		sr_set_saddr(sock, addr);
	}

	sk_set_connection_state(sock, CONNECTING);

	ret = sock->sk->f_ops->connect(sock->sk, (struct sockaddr *)addr,
			addrlen, flags);
	if (ret != 0) {
		LOG_ERROR("ksocket_connect", "unable to connect on socket");
		sk_set_connection_state(sock, BOUND);
		return ret;
	}

	sk_set_connection_state(sock, CONNECTED);

	return 0;
}

int klisten(struct socket *sock, int backlog) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	backlog = backlog > 0 ? backlog : 1;

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (!sk_is_bound(sock)) {
		return -EDESTADDRREQ;
	}
	else if (sk_is_connected(sock)){
		return -EINVAL;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->listen == NULL) {
		return -ENOSYS;
	}

	assert(sock->sk != NULL);
	ret = sock->sk->f_ops->listen(sock->sk, backlog);
	if (ret != 0) {
		LOG_ERROR("ksocket_listen",
				"error setting socket in listening state");
		sk_set_connection_state(sock, BOUND);
		return ret;
	}

	sk_set_connection_state(sock, LISTENING);
	sock->sk->opt.so_acceptconn = 1;

	return 0;
}

int kaccept(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct socket **out_sock) {
	int ret;
	struct sock *new_sk;
	struct socket *new_sock;

	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0) || (out_sock == NULL)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (!sk_is_listening(sock)) {
		LOG_ERROR("ksocket_accept",
				"accepting socket should be in listening state");
		return -EINVAL;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->accept == NULL) {
		return -EOPNOTSUPP;
	}

	ret = sock->sk->f_ops->accept(sock->sk, addr, addrlen,
			flags, &new_sk);
	if (ret != 0) {
		LOG_ERROR("ksocket_accept",
				"error while accepting a connection");
		return ret;
	}

	ret = ksocket_ext(sock->sk->opt.so_domain,
			sock->sk->opt.so_type, sock->sk->opt.so_protocol,
			new_sk, &new_sock);
	if (ret != 0) {
		sock_release(new_sk);
		return ret;
	}

	sk_set_connection_state(new_sock, ESTABLISHED);

	*out_sock = new_sock;

	return 0;
}

int ksendmsg(struct socket *sock, struct msghdr *msg, int flags) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if (msg == NULL) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	switch (sock->sk->opt.so_type) {
	default:
		if (msg->msg_name == NULL) {
			if (msg->msg_namelen != 0) {
				return -EINVAL;
			}
			else if (!sk_is_connected(sock)) {
				return -EDESTADDRREQ;
			}
		}
		else if (msg->msg_namelen <= 0) {
			return -EINVAL;
		}
		break;
	case SOCK_STREAM:
		if (!sk_is_connected(sock)) {
			return -ENOTCONN;
		}
		else if ((msg->msg_name != NULL)
				|| (msg->msg_namelen != 0)) {
			return -EISCONN;
		}
		break;
	}

	if (sock->sk->shutdown_flag & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	if (msg->msg_flags != 0) { /* TODO remove this */
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->sendmsg == NULL) {
		return -ENOSYS;
	}

	return sock->sk->f_ops->sendmsg(sock->sk, msg, flags);
}

int krecvmsg(struct socket *sock, struct msghdr *msg, int flags) {
	int ret;

	if (sock == NULL) {
		return -EBADF;
	}
	else if (msg == NULL) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if ((sock->sk->opt.so_type == SOCK_STREAM)
			&& !sk_is_connected(sock)) {
		return -ENOTCONN;
	}

	if (sock->sk->shutdown_flag & (SHUT_WR + 1)) {
		return -EPIPE;
	}

	if (msg->msg_flags != 0) { /* TODO remove this */
		LOG_ERROR("ksendmsg", "flags are not supported");
		return -EOPNOTSUPP;
	}

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	ret = sock->sk->f_ops->recvmsg(sock->sk, msg, flags);
	if ((ret == -EAGAIN) && !(flags & O_NONBLOCK)) {
		EVENT_WAIT(&sock->sk->sock_is_not_empty, 0,
				SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
		ret = sock->sk->f_ops->recvmsg(sock->sk, msg, flags);
	}

	return ret;
}

int kshutdown(struct socket *sock, int how) {
	if (sock == NULL) {
		return -EBADF;
	}
	switch (how) {
	default:
		return -EINVAL;
	case SHUT_RD:
	case SHUT_WR:
	case SHUT_RDWR:
		break;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (!sk_is_connected(sock)){
		return -ENOTCONN;
	}

	sock->sk->shutdown_flag |= (how + 1);

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->shutdown == NULL) {
		return 0;
	}

	return sock->sk->f_ops->shutdown(sock->sk, how);
}

int kgetsockname(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->getsockname == NULL) {
		return -ENOSYS;
	}

	return sock->sk->f_ops->getsockname(sock->sk, addr, addrlen);
}

int kgetpeername(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((addr == NULL) || (addrlen == NULL)
			|| (*addrlen <= 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	assert(sock->sk->f_ops != NULL);
	if (sock->sk->f_ops->getpeername == NULL) {
		return -ENOSYS;
	}

	return sock->sk->f_ops->getpeername(sock->sk, addr, addrlen);
}

#define CASE_GETSOCKOPT(test_name, field, expression)       \
	case test_name:                                         \
		memcpy(optval, &sock->sk->opt.field,                \
				min(*optlen, sizeof sock->sk->opt.field));  \
		expression;                                         \
		*optlen = min(*optlen, sizeof sock->sk->opt.field); \
		break

int kgetsockopt(struct socket *sock, int level, int optname,
		void *optval, socklen_t *optlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (level != SOL_SOCKET) {
		assert(sock->sk->f_ops != NULL);
		if (sock->sk->f_ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sock->sk->f_ops->getsockopt(sock->sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	CASE_GETSOCKOPT(SO_ACCEPTCONN, so_acceptconn, );
	CASE_GETSOCKOPT(SO_BINDTODEVICE, so_bindtodevice,
			strncpy(optval, &sock->sk->opt.so_bindtodevice->name[0],
				*optlen);
			*optlen = min(*optlen,
				strlen(&sock->sk->opt.so_bindtodevice->name[0]) + 1);
			break);
	CASE_GETSOCKOPT(SO_BROADCAST, so_broadcast, );
	CASE_GETSOCKOPT(SO_DOMAIN, so_domain, );
	CASE_GETSOCKOPT(SO_DONTROUTE, so_dontroute, );
	CASE_GETSOCKOPT(SO_ERROR, so_error,
			sock->sk->opt.so_error = 0);
	CASE_GETSOCKOPT(SO_LINGER, so_linger, );
	CASE_GETSOCKOPT(SO_OOBINLINE, so_oobinline, );
	CASE_GETSOCKOPT(SO_PROTOCOL, so_protocol, );
	CASE_GETSOCKOPT(SO_RCVBUF, so_rcvbuf, );
	CASE_GETSOCKOPT(SO_RCVLOWAT, so_rcvlowat, );
	CASE_GETSOCKOPT(SO_RCVTIMEO, so_rcvtimeo,
			if (*optlen > sizeof sock->sk->opt.so_rcvtimeo) {
				return -EDOM;
			});
	CASE_GETSOCKOPT(SO_SNDBUF, so_sndbuf, );
	CASE_GETSOCKOPT(SO_SNDLOWAT, so_sndlowat, );
	CASE_GETSOCKOPT(SO_SNDTIMEO, so_sndtimeo,
			if (*optlen > sizeof sock->sk->opt.so_sndtimeo) {
				return -EDOM;
			});
	CASE_GETSOCKOPT(SO_TYPE, so_type, );
	}

	return 0;
}

#define CASE_SETSOCKOPT(test_name, field, expression) \
	case test_name:                                   \
		expression;                                   \
		if (optlen != sizeof sock->sk->opt.field) {   \
			return -EINVAL;                           \
		}                                             \
		memcpy(&sock->sk->opt.field, optval, optlen); \
		break

int ksetsockopt(struct socket *sock, int level, int optname,
		const void *optval, socklen_t optlen) {
	if (sock == NULL) {
		return -EBADF;
	}
	else if ((optval == NULL) || (optlen < 0)) {
		return -EINVAL;
	}

	assert(sr_socket_exists(sock));
	assert(sock->sk != NULL);

	if (level != SOL_SOCKET) {
		assert(sock->sk->f_ops != NULL);
		if (sock->sk->f_ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sock->sk->f_ops->setsockopt(sock->sk, level, optname,
				optval, optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	case SO_ACCEPTCONN:
	case SO_DOMAIN:
	case SO_ERROR:
	case SO_PROTOCOL:
	case SO_TYPE:
		return -EINVAL;
	CASE_SETSOCKOPT(SO_BINDTODEVICE, so_bindtodevice,
			optval = netdev_get_by_name(optval);
			if (optval == NULL) {
				return -ENODEV;
			});
	CASE_SETSOCKOPT(SO_BROADCAST, so_broadcast, );
	CASE_SETSOCKOPT(SO_DONTROUTE, so_dontroute, );
	CASE_SETSOCKOPT(SO_LINGER, so_linger, );
	CASE_SETSOCKOPT(SO_OOBINLINE, so_oobinline, );
	CASE_SETSOCKOPT(SO_RCVBUF, so_rcvbuf, );
	CASE_SETSOCKOPT(SO_RCVLOWAT, so_rcvlowat, );
	CASE_SETSOCKOPT(SO_RCVTIMEO, so_rcvtimeo,
			if (optlen > sizeof sock->sk->opt.so_rcvtimeo) {
				return -EDOM;
			});
	CASE_SETSOCKOPT(SO_SNDBUF, so_sndbuf, );
	CASE_SETSOCKOPT(SO_SNDLOWAT, so_sndlowat, );
	CASE_SETSOCKOPT(SO_SNDTIMEO, so_sndtimeo,
			if (optlen > sizeof sock->sk->opt.so_sndtimeo) {
				return -EDOM;
			});
	}

	return 0;
}
