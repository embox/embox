/**
 * @file
 * @brief
 *
 * @date 28.10.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>


#include <net/family.h>
#include <net/net_sock.h>

#include <mem/misc/pool.h>

#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/l3/ipv6.h>
#include <net/sock.h>

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

EMBOX_NET_FAMILY(AF_INET6, inet6_types, ip6_out_ops);

static int inet6_init(struct sock *sk) {
	struct inet6_sock *in6_sk;

	assert(sk);

	in6_sk = to_inet6_sock(sk);
	memset(&in6_sk->src_in6, 0, sizeof in6_sk->src_in6);
	memset(&in6_sk->dst_in6, 0, sizeof in6_sk->dst_in6);
	in6_sk->src_in6.sin6_family = in6_sk->dst_in6.sin6_family = AF_UNSPEC;

	in6_sk->sk.src_addr = (const struct sockaddr *)&in6_sk->src_in6;
	in6_sk->sk.dst_addr = (const struct sockaddr *)&in6_sk->dst_in6;
	in6_sk->sk.addr_len = sizeof(struct sockaddr_in6);

	return 0;
}

static int inet6_close(struct sock *sk) {
	assert(sk);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->close == NULL) {
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

static int inet6_addr_tester(const struct sockaddr *lhs_sa,
		const struct sockaddr *rhs_sa) {
	const struct sockaddr_in6 *lhs_in6, *rhs_in6;

	assert(lhs_sa != NULL);
	assert(rhs_sa != NULL);

	lhs_in6 = (const struct sockaddr_in6 *)lhs_sa;
	rhs_in6 = (const struct sockaddr_in6 *)rhs_sa;

	assert(lhs_in6->sin6_family == AF_INET6);
	return (lhs_in6->sin6_family == rhs_in6->sin6_family)
			&& ((0 == memcmp(&lhs_in6->sin6_addr, &rhs_in6->sin6_addr,
							sizeof lhs_in6->sin6_addr))
					|| (0 == memcmp(&lhs_in6->sin6_addr, &in6addr_any,
							sizeof lhs_in6->sin6_addr))
					|| (0 == memcmp(&rhs_in6->sin6_addr, &in6addr_any,
							sizeof rhs_in6->sin6_addr)))
			&& (lhs_in6->sin6_port == rhs_in6->sin6_port);
}

static int inet6_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	const struct sockaddr_in6 *addr_in6;

	assert(sk);
	assert(addr);

	if (addrlen != sizeof *addr_in6) {
		return -EINVAL;
	}

	addr_in6 = (const struct sockaddr_in6 *)addr;
	if (addr_in6->sin6_family != AF_INET6) {
		return -EAFNOSUPPORT;
	}
	else if ((0 != memcmp(&addr_in6->sin6_addr, &in6addr_any,
				sizeof addr_in6->sin6_addr))
			&& (0 != memcmp(&addr_in6->sin6_addr,
				&in6addr_loopback, sizeof addr_in6->sin6_addr))) {
		/* FIXME */
		return -EADDRNOTAVAIL;
	}
	else if (sock_addr_is_busy(sk->p_ops, inet6_addr_tester, addr,
				addrlen)) {
		return -EADDRINUSE;
	}

	__inet6_bind(to_inet6_sock(sk), addr_in6);

	return 0;
}

static int inet6_bind_local(struct sock *sk) {
	struct sockaddr_in6 addr_in6;

	assert(sk);

	addr_in6.sin6_family = AF_INET6;
	memcpy(&addr_in6.sin6_addr, &in6addr_loopback,
			sizeof addr_in6.sin6_addr);

	if (!sock_addr_alloc_port(sk->p_ops, &addr_in6.sin6_port,
				inet6_addr_tester, (const struct sockaddr *)&addr_in6,
				sizeof addr_in6)) {
		return -ENOMEM;
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

	assert(sk);
	assert(addr);

	if (addrlen != sizeof *addr_in6) {
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
		return -EOPNOTSUPP;
	}

	return sk->p_ops->connect(sk, addr, addrlen, flags);
}

static int inet6_nonstream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	int ret;
	const struct sockaddr_in6 *addr_in6;

	assert(sk);
	assert(addr);

	if (addrlen != sizeof *addr_in6) {
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
	assert(sk);
	assert(backlog >= 0);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->listen == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->listen(sk, backlog);
}

static int inet6_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct sock **out_sk) {
	int ret;
	struct sockaddr_in6 *addr_in6;

	assert(sk);
	assert(out_sk);

	addr_in6 = (struct sockaddr_in6 *)addr;
	if (((addr_in6 == NULL) && (addrlen != NULL))
			|| ((addr_in6 != NULL) && (addrlen == NULL))
			|| ((addrlen != NULL) && (*addrlen < sizeof *addr_in6))) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->accept == NULL) {
		return -EOPNOTSUPP;
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

	assert(sk);
	assert(msg);

	addr_in6 = (const struct sockaddr_in6 *)msg->msg_name;
	if ((addr_in6 != NULL) &&
			((msg->msg_namelen != sizeof *addr_in6)
				|| (addr_in6->sin6_family != AF_INET6))) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sendmsg == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->sendmsg(sk, msg, flags);
}

static int inet6_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	int ret;
	struct sockaddr_in6 *addr_in6;

	assert(sk);
	assert(msg);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->recvmsg == NULL) {
		return -EOPNOTSUPP;
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

	assert(sk);
	assert(addr);
	assert(addrlen);

	if (*addrlen < sizeof *addr_in6) {
		return -EINVAL;
	}

	addr_in6 = (struct sockaddr_in6 *)addr;
	memcpy(addr_in6, &to_inet6_sock(sk)->src_in6, sizeof *addr_in6);
#if 0
	assert((addr_in6->sin6_family == AF_UNSPEC)
			|| (addr_in6->sin6_family == AF_INET6));
#else
	addr_in6->sin6_family = AF_INET6;
#endif
	*addrlen = sizeof *addr_in6;

	return 0;
}

static int inet6_getpeername(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct sockaddr_in6 *addr_in6;

	assert(sk);
	assert(addr);
	assert(addrlen);

	if (*addrlen < sizeof *addr_in6) {
		return -EINVAL;
	}

	addr_in6 = (struct sockaddr_in6 *)addr;
	memcpy(addr_in6, &to_inet6_sock(sk)->dst_in6, sizeof *addr_in6);
#if 0
	assert((addr_in6->sin6_family == AF_UNSPEC)
			|| (addr_in6->sin6_family == AF_INET6));
#else
	addr_in6->sin6_family = AF_INET6;
#endif
	*addrlen = sizeof *addr_in6;

	return 0;
}

static int inet6_getsockopt(struct sock *sk, int level,
		int optname, void *optval, socklen_t *optlen) {
	assert(sk);
	assert(optval);
	assert(optlen);
	assert(*optlen >= 0);

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
	assert(sk);
	assert(optval);
	assert(optlen >= 0);

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
	assert(sk);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->shutdown == NULL) {
		return -EOPNOTSUPP;
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
