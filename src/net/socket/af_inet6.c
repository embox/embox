/**
 * @file
 * @brief
 *
 * @date 28.10.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/net/sock/api.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <net/sock.h>
#include <sys/socket.h>
#include <net/socket/inet6_sock.h>
#include <net/inetdevice.h>
#include <net/if_ether.h>
#include <embox/net/family.h>
#include <util/indexator.h>
#include <embox/net/sock.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <net/l3/route.h>

#include <net/socket/inet6_sock.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_INET_SOCK OPTION_GET(NUMBER, amount_inet6_sock)

static const struct sock_family_ops inet6_stream_ops;
static const struct sock_family_ops inet6_dgram_ops;
static const struct sock_family_ops inet6_raw_ops;

static const struct net_family_type inet6_types[] = {
	{ SOCK_STREAM, &inet6_stream_ops },
	{ SOCK_DGRAM, &inet6_dgram_ops },
	{ SOCK_RAW, &inet6_raw_ops }
};

EMBOX_NET_FAMILY(AF_INET6, inet6_types);

static int inet6_init(struct sock *sk) {
	struct inet6_sock *in6_sk;

	if (sk == NULL) {
		return -EINVAL;
	}

	in6_sk = to_inet6_sock(sk);
	in6_sk->src_port_alloced = 0;
	memset(&in6_sk->src_in6, 0, sizeof in6_sk->src_in6);
	memset(&in6_sk->dst_in6, 0, sizeof in6_sk->dst_in6);
	in6_sk->src_in6.sin6_family = in6_sk->dst_in6.sin6_family = AF_INET6;

#if 0
	in6_sk->sk.src_addr = &in6_sk->src_in6;
	in6_sk->sk.dst_addr = &in6_sk->dst_in6;
	in6_sk->sk.addr_len = sizeof(struct sockaddr_in6);
#endif

	return 0;
}

static int inet6_close(struct sock *sk) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->close == NULL) {
		if (to_inet6_sock(sk)->src_port_alloced) {
			assert(to_inet6_sock(sk)->src_in6.sin6_family == AF_INET6);
			index_unlock(sk->p_ops->sock_port,
					ntohs(to_inet6_sock(sk)->src_in6.sin6_port));
		}
		sock_release(sk);
		return 0;
	}

	return sk->p_ops->close(sk);
}

static void __inet6_bind(struct inet6_sock *in6_sk,
		const struct sockaddr_in6 *addr_in6) {
	assert(in6_sk != NULL);
	assert(addr_in6 != NULL);
	assert(addr_in6->sin6_family == AF_INET6);
	memcpy(&in6_sk->src_in6, addr_in6, sizeof *addr_in6);
}

static int inet6_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	const struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (addr == NULL)
			|| (addrlen != sizeof *addr_in6)) {
		return -EINVAL;
	}

	addr_in6 = (const struct sockaddr_in6 *)addr;
	if (addr_in6->sin6_family != AF_INET6) {
		return -EAFNOSUPPORT;
	}
	else if ((0 != memcmp(&addr_in6->sin6_addr, &in6addr_any,
				sizeof addr_in6->sin6_addr))
			|| (0 != memcmp(&addr_in6->sin6_addr, &in6addr_loopback,
					sizeof addr_in6->sin6_addr))) {
		/* FIXME */
		return -EADDRNOTAVAIL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sock_port != NULL) {
		if (!index_try_lock(sk->p_ops->sock_port,
					ntohs(addr_in6->sin6_port))) {
			return -EADDRINUSE;
		}
		to_inet6_sock(sk)->src_port_alloced = 1;
	}

	__inet6_bind(to_inet6_sock(sk), addr_in6);

	return 0;
}

static int inet6_bind_local(struct sock *sk) {
	size_t port;
	struct sockaddr_in6 addr_in6;

	if (sk == NULL) {
		return -EINVAL;
	}

	addr_in6.sin6_family = AF_INET6;
	memcpy(&addr_in6.sin6_addr, &in6addr_loopback,
			sizeof addr_in6.sin6_addr);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sock_port != NULL) {
		port = index_alloc(sk->p_ops->sock_port, INDEX_NEXT);
		if (port == INDEX_NONE) {
			return -ENOMEM;
		}
		to_inet6_sock(sk)->src_port_alloced = 1;
		addr_in6.sin6_port = htons(port);
	}
	else {
		addr_in6.sin6_port = 0;
	}

	__inet6_bind(to_inet6_sock(sk), &addr_in6);

	return 0;
}

static int __inet6_connect(struct inet6_sock *in6_sk,
		const struct sockaddr_in6 *addr_in6) {
//	int ret;
//	in_addr_t src_ip;

	assert(in6_sk != NULL);
	assert(addr_in6 != NULL);
	assert(addr_in6->sin6_family == AF_INET6);

#if 0
	/* FIXME */
	ret = rt_fib_source_ip(addr_in6->sin6_addr.s_addr, &src_ip);
	if (ret != 0) {
		return ret;
	}

	if ((in6_sk->src_in6.sin6_addr.s_addr != htonl(INADDR_ANY))
			&& (in6_sk->src_in6.sin6_addr.s_addr != src_ip)) {
		return -EINVAL;
	}

	in6_sk->src_in6.sin6_addr.s_addr = src_ip;
#endif

	memcpy(&in6_sk->dst_in6, addr_in6, sizeof *addr_in6);

	return 0;
}

static int inet6_stream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	int ret;
	const struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (addr == NULL)
			|| (addrlen != sizeof *addr_in6)) {
		return -EINVAL;
	}

	addr_in6 = (const struct sockaddr_in6 *)addr;
	if (addr_in6->sin6_family != AF_INET6) {
		return -EINVAL;
	}

	ret = __inet6_connect(to_inet6_sock(sk), addr_in6);
	if (ret != 0) {
		return ret;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->connect == NULL) {
		return -ENOSYS;
	}

	return sk->p_ops->connect(sk, addr, addrlen, flags);
}

static int inet6_nonstream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	int ret;
	const struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (addr == NULL)
			|| (addrlen != sizeof *addr_in6)) {
		return -EINVAL;
	}

	addr_in6 = (const struct sockaddr_in6 *)addr;
	if (addr_in6->sin6_family != AF_INET6) {
		return -EINVAL;
	}

	ret = __inet6_connect(to_inet6_sock(sk), addr_in6);
	if (ret != 0) {
		return ret;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->connect == NULL) {
		return 0;
	}

	return sk->p_ops->connect(sk, addr, addrlen, flags);
}

static int inet6_listen(struct sock *sk, int backlog) {
	if ((sk == NULL) || (backlog < 0)) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->listen == NULL) {
		return -ENOSYS;
	}

	return sk->p_ops->listen(sk, backlog);
}

static int inet6_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct sock **out_sk) {
	int ret;
	struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (out_sk == NULL)) {
		return -EINVAL;
	}

	addr_in6 = (struct sockaddr_in6 *)addr;
	if (((addr_in6 == NULL) && (addrlen != NULL))
			|| ((addr_in6 != NULL) && (addrlen == NULL))
			|| ((addrlen != NULL) && (*addrlen < sizeof *addr_in6))) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->accept == NULL) {
		return -ENOSYS;
	}

	ret = sk->p_ops->accept(sk, addr, addrlen, flags, out_sk);
	if (ret != 0) {
		return ret;
	}

	if (addr_in6 != NULL) {
		memcpy(addr_in6, &to_inet6_sock(*out_sk)->dst_in6, sizeof *addr_in6);
		assert(addr_in6->sin6_family == AF_INET6);
		*addrlen = sizeof *addr_in6;
	}

	return 0;
}

static int inet6_sendmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	const struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	addr_in6 = (const struct sockaddr_in6 *)msg->msg_name;
	if ((addr_in6 != NULL) &&
			((msg->msg_namelen != sizeof *addr_in6)
				|| (addr_in6->sin6_family != AF_INET6))) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sendmsg == NULL) {
		return -ENOSYS;
	}

	return sk->p_ops->sendmsg(sk, msg, flags);
}

static int inet6_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	int ret;
	struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	ret = sk->p_ops->recvmsg(sk, msg, flags);
	if (ret != 0) {
		return ret;
	}

	if (msg->msg_name != NULL) {
		if (msg->msg_namelen < sizeof *addr_in6) {
			return -EINVAL;
		}
		addr_in6 = (struct sockaddr_in6 *)msg->msg_name;
		memcpy(addr_in6, &to_inet6_sock(sk)->dst_in6, sizeof *addr_in6);
		assert(addr_in6->sin6_family == AF_INET6);
		msg->msg_namelen = sizeof *addr_in6;
	}

	return 0;
}

static int inet6_getsockname(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (addr == NULL) || (addrlen == NULL)
			|| (*addrlen < sizeof *addr_in6)) {
		return -EINVAL;
	}

	addr_in6 = (struct sockaddr_in6 *)addr;
	memcpy(addr_in6, &to_inet6_sock(sk)->src_in6, sizeof *addr_in6);
	assert(addr_in6->sin6_family == AF_INET6);
	*addrlen = sizeof *addr_in6;

	return 0;
}

static int inet6_getpeername(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct sockaddr_in6 *addr_in6;

	if ((sk == NULL) || (addr == NULL) || (addrlen == NULL)
			|| (*addrlen < sizeof *addr_in6)) {
		return -EINVAL;
	}

	addr_in6 = (struct sockaddr_in6 *)addr;
	memcpy(addr_in6, &to_inet6_sock(sk)->dst_in6, sizeof *addr_in6);
	assert(addr_in6->sin6_family == AF_INET6);
	*addrlen = sizeof *addr_in6;

	return 0;
}

static int inet6_getsockopt(struct sock *sk, int level,
		int optname, void *optval, socklen_t *optlen) {
	if ((sk == NULL) || (optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return -EINVAL;
	}

	if (level != IPPROTO_IPV6) {
		assert(sk->p_ops != NULL);
		if (sk->p_ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->p_ops->getsockopt(sk, level, optname, optval,
				optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

static int inet6_setsockopt(struct sock *sk, int level,
		int optname, const void *optval, socklen_t optlen) {
	if ((sk == NULL) || (optval == NULL) || (optlen < 0)) {
		return -EINVAL;
	}

	if (level != IPPROTO_IPV6) {
		assert(sk->p_ops != NULL);
		if (sk->p_ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->p_ops->setsockopt(sk, level, optname, optval,
				optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

static int inet6_shutdown(struct sock *sk, int how) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->shutdown == NULL) {
		return 0;
	}

	return sk->p_ops->shutdown(sk, how);
}

POOL_DEF(inet6_sock_pool, struct inet6_sock, MODOPS_AMOUNT_INET_SOCK);

static const struct sock_family_ops inet6_stream_ops = {
	.init        = inet6_init,
	.close       = inet6_close,
	.bind        = inet6_bind,
	.bind_local  = inet6_bind_local,
	.connect     = inet6_stream_connect,
	.listen      = inet6_listen,
	.accept      = inet6_accept,
	.sendmsg     = inet6_sendmsg,
	.recvmsg     = inet6_recvmsg,
	.getsockname = inet6_getsockname,
	.getpeername = inet6_getpeername,
	.getsockopt  = inet6_getsockopt,
	.setsockopt  = inet6_setsockopt,
	.shutdown    = inet6_shutdown,
	.sock_pool   = &inet6_sock_pool
};

static const struct sock_family_ops inet6_dgram_ops = {
	.init        = inet6_init,
	.close       = inet6_close,
	.bind        = inet6_bind,
	.bind_local  = inet6_bind_local,
	.connect     = inet6_nonstream_connect,
	.listen      = inet6_listen,
	.accept      = inet6_accept,
	.sendmsg     = inet6_sendmsg,
	.recvmsg     = inet6_recvmsg,
	.getsockname = inet6_getsockname,
	.getpeername = inet6_getpeername,
	.getsockopt  = inet6_getsockopt,
	.setsockopt  = inet6_setsockopt,
	.shutdown    = inet6_shutdown,
	.sock_pool   = &inet6_sock_pool
};

static const struct sock_family_ops inet6_raw_ops = {
	.init        = inet6_init,
	.close       = inet6_close,
	.bind        = inet6_bind,
	.bind_local  = inet6_bind_local,
	.connect     = inet6_nonstream_connect,
	.listen      = inet6_listen,
	.accept      = inet6_accept,
	.sendmsg     = inet6_sendmsg,
	.recvmsg     = inet6_recvmsg,
	.getsockname = inet6_getsockname,
	.getpeername = inet6_getpeername,
	.getsockopt  = inet6_getsockopt,
	.setsockopt  = inet6_setsockopt,
	.shutdown    = inet6_shutdown,
	.sock_pool   = &inet6_sock_pool
};
