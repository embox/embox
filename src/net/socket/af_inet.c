/**
 * @file
 * @brief PF_INET protocol family socket handler.
 *
 * @date 01.12.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <errno.h>

#include <stddef.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

#include <net/sock.h>
#include <net/inetdevice.h>

#include <net/family.h>
#include <net/net_sock.h>

#include <mem/misc/pool.h>

#include <net/l3/route.h>
#include <net/l3/ipv4/ip.h>

#include <net/socket/inet_sock.h>
#include <framework/mod/options.h>

#define MODOPS_AMOUNT_INET_SOCK OPTION_GET(NUMBER, amount_inet_sock)

static const struct sock_family_ops inet_stream_ops;
static const struct sock_family_ops inet_dgram_ops;
static const struct sock_family_ops inet_raw_ops;

static const struct net_family_type inet_types[] = {
	{ SOCK_STREAM, &inet_stream_ops },
	{ SOCK_DGRAM, &inet_dgram_ops },
	{ SOCK_RAW, &inet_raw_ops }
};

EMBOX_NET_FAMILY(AF_INET, inet_types, ip_out_ops);

static int inet_init(struct sock *sk) {
	struct inet_sock *in_sk;

	assert(sk);

	in_sk = to_inet_sock(sk);
	in_sk->id = 0;
	in_sk->uc_ttl = -1;
	memset(&in_sk->src_in, 0, sizeof in_sk->src_in);
	memset(&in_sk->dst_in, 0, sizeof in_sk->dst_in);
	in_sk->src_in.sin_family = in_sk->dst_in.sin_family = AF_INET;

	in_sk->sk.src_addr = (const struct sockaddr *)&in_sk->src_in;
	in_sk->sk.dst_addr = (const struct sockaddr *)&in_sk->dst_in;
	in_sk->sk.addr_len = sizeof(struct sockaddr_in);
	memset(&in_sk->opt, 0, sizeof in_sk->opt);

	return 0;
}

static int inet_close(struct sock *sk) {
	assert(sk);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->close == NULL) {
		sock_release(sk);
		return 0;
	}

	return sk->p_ops->close(sk);
}

static void __inet_bind(struct inet_sock *in_sk,
		const struct sockaddr_in *addr_in) {
	assert(in_sk != NULL);
	assert(addr_in != NULL);
	assert(addr_in->sin_family == AF_INET);
	memcpy(&in_sk->src_in, addr_in, sizeof *addr_in);
}

static int inet_addr_tester(const struct sockaddr *lhs_sa,
		const struct sockaddr *rhs_sa) {
	static const struct in_addr inaddr_any = {
		{ .s_addr = htonl(INADDR_ANY) }
	};
	const struct sockaddr_in *lhs_in, *rhs_in;

	assert(lhs_sa != NULL);
	assert(rhs_sa != NULL);

	lhs_in = (const struct sockaddr_in *)lhs_sa;
	rhs_in = (const struct sockaddr_in *)rhs_sa;

	assert(lhs_in->sin_family == AF_INET);
	return (lhs_in->sin_family == rhs_in->sin_family)
			&& ((0 == memcmp(&lhs_in->sin_addr, &rhs_in->sin_addr,
							sizeof lhs_in->sin_addr))
					|| (0 == memcmp(&lhs_in->sin_addr, &inaddr_any,
							sizeof lhs_in->sin_addr))
					|| (0 == memcmp(&rhs_in->sin_addr, &inaddr_any,
							sizeof rhs_in->sin_addr)))
			&& (lhs_in->sin_port == rhs_in->sin_port);
}
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
int sock_addr_is_busy_net_ns(const struct sock_proto_ops *p_ops,
		sock_addr_tester_ft tester, const struct sockaddr *addr,
		socklen_t addrlen, struct sock *src_sk) {
	const struct sock *sk;

	assert(p_ops != NULL);
	assert(tester != NULL);

	sock_foreach(sk, p_ops) {
		if (!cmp_net_ns(src_sk->net_ns, sk->net_ns)) {
			continue;
		}
		if ((sk->addr_len == addrlen)
				&& tester(addr, sk->src_addr)) {
			return 1;
		}
	}

	return 0;
}
#endif

static int inet_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	struct sockaddr_in addr_in;

	assert(sk);
	assert(addr);

	if (addrlen != sizeof addr_in) {
		return -EINVAL;
	}

	memcpy(&addr_in, addr, sizeof addr_in);

	if (addr_in.sin_family != AF_INET) {
		return -EAFNOSUPPORT;
	}
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	else if ((addr_in.sin_addr.s_addr != htonl(INADDR_ANY)) &&
			!ip_is_local_net_ns(addr_in.sin_addr.s_addr,
								IP_LOCAL_BROADCAST | IP_LOCAL_MULTICAST,
								sk->net_ns)) {
		return -EADDRNOTAVAIL;
	}
#else
	else if ((addr_in.sin_addr.s_addr != htonl(INADDR_ANY)) &&
			!ip_is_local(addr_in.sin_addr.s_addr, IP_LOCAL_BROADCAST | IP_LOCAL_MULTICAST)) {
		return -EADDRNOTAVAIL;
	}
#endif

	if (addr_in.sin_port == 0) {
		/* Allocation of an ephemeral port, when the port number in a socket address is specified as 0,
		 * is not POSIX-specified behavior, but it used in Linux, BSD, Windows.
		 * E.g. manual for Linux - http://man7.org/linux/man-pages/man7/ip.7.html */
		if (!sock_addr_alloc_port(sk->p_ops, &addr_in.sin_port,
					inet_addr_tester, (const struct sockaddr *)&addr_in,
					sizeof addr_in)) {
			return -ENOMEM;
		}
	}
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	else if (sock_addr_is_busy_net_ns(sk->p_ops, inet_addr_tester, (struct sockaddr *)&addr_in,
				addrlen, sk)) {
		/* TODO consider opt.so_reuseaddr */
		return -EADDRINUSE;
	}
#else
	else if (sock_addr_is_busy(sk->p_ops, inet_addr_tester, (struct sockaddr *)&addr_in,
			addrlen)) {
		/* TODO consider opt.so_reuseaddr */
		return -EADDRINUSE;
	}
#endif
	__inet_bind(to_inet_sock(sk), &addr_in);

	return 0;
}

static int inet_bind_local(struct sock *sk) {
	struct sockaddr_in addr_in;

	assert(sk);

	memset(&addr_in, 0, sizeof addr_in);

	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	if (!sock_addr_alloc_port(sk->p_ops, &addr_in.sin_port,
				inet_addr_tester, (const struct sockaddr *)&addr_in,
				sizeof addr_in)) {
		return -ENOMEM;
	}

	__inet_bind(to_inet_sock(sk), &addr_in);

	return 0;
}

static int __inet_connect(struct inet_sock *in_sk,
		const struct sockaddr_in *addr_in) {
	int ret;
	in_addr_t src_ip;

	assert(in_sk != NULL);
	assert(addr_in != NULL);
	assert(addr_in->sin_family == AF_INET);

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	ret = rt_fib_source_ip_net_ns(addr_in->sin_addr.s_addr, NULL, &src_ip,
				((struct sock *)in_sk)->net_ns);
#else
	ret = rt_fib_source_ip(addr_in->sin_addr.s_addr, NULL, &src_ip);
#endif

	if (ret != 0) {
		return ret;
	}

	if ((in_sk->src_in.sin_addr.s_addr != htonl(INADDR_ANY))
			&& (in_sk->src_in.sin_addr.s_addr != src_ip)) {
		return -EINVAL;
	}

	in_sk->src_in.sin_addr.s_addr = src_ip;

	memcpy(&in_sk->dst_in, addr_in, sizeof *addr_in);

	return 0;
}

static int inet_stream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	int ret;
	const struct sockaddr_in *addr_in;

	assert(sk);
	assert(addr);

	if (addrlen != sizeof *addr_in) {
		return -EINVAL;
	}

	addr_in = (const struct sockaddr_in *)addr;
	if (addr_in->sin_family != AF_INET) {
		return -EINVAL;
	}

	ret = __inet_connect(to_inet_sock(sk), addr_in);
	if (ret != 0) {
		return ret;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->connect == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->connect(sk, addr, addrlen, flags);
}

static int inet_nonstream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	int ret;
	const struct sockaddr_in *addr_in;

	assert(sk);
	assert(addr);

	if (addrlen != sizeof *addr_in) {
		return -EINVAL;
	}

	addr_in = (const struct sockaddr_in *)addr;
	if (addr_in->sin_family != AF_INET) {
		return -EINVAL;
	}

	ret = __inet_connect(to_inet_sock(sk), addr_in);
	if (ret != 0) {
		return ret;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->connect == NULL) {
		return 0;
	}

	return sk->p_ops->connect(sk, addr, addrlen, flags);
}

static int inet_listen(struct sock *sk, int backlog) {
	assert(sk);
	assert(backlog >= 0);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->listen == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->listen(sk, backlog);
}

static int inet_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct sock **out_sk) {
	int ret;
	struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;

	assert(sk);
	assert(out_sk);
	assert(addr || !addrlen);
	assert(!addr || addrlen);

	if (addrlen != NULL && *addrlen < sizeof *addr_in) {
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

	if (addr_in != NULL) {
		memcpy(addr_in, &to_inet_sock(*out_sk)->dst_in, sizeof *addr_in);
		assert(addr_in->sin_family == AF_INET);
		*addrlen = sizeof *addr_in;
	}

	return 0;
}

static int inet_sendmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	const struct sockaddr_in *addr_in;

	assert(sk);
	assert(msg);

	addr_in = (const struct sockaddr_in *)msg->msg_name;
	if ((addr_in != NULL) &&
			((msg->msg_namelen != sizeof *addr_in)
				|| (addr_in->sin_family != AF_INET))) {
		return -EINVAL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sendmsg == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->sendmsg(sk, msg, flags);
}

static int inet_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	struct sockaddr_in *addr_in;

	assert(sk);
	assert(msg);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	if (msg->msg_name != NULL) {
		if (msg->msg_namelen < sizeof *addr_in) {
			return -EINVAL;
		}
		addr_in = (struct sockaddr_in *)msg->msg_name;
		memcpy(addr_in, &to_inet_sock(sk)->dst_in, sizeof *addr_in);
		assert(addr_in->sin_family == AF_INET);
		msg->msg_namelen = sizeof *addr_in;
	}

	return sk->p_ops->recvmsg(sk, msg, flags);
}

static int inet_getsockname(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct sockaddr_in *addr_in;

	assert(sk);
	assert(addr);
	assert(addrlen);

	if (*addrlen < sizeof *addr_in) {
		return -EINVAL;
	}

	addr_in = (struct sockaddr_in *)addr;
	memcpy(addr_in, &to_inet_sock(sk)->src_in, sizeof *addr_in);
#if 0
	assert((addr_in->sin_family == AF_UNSPEC)
			|| (addr_in->sin_family == AF_INET));
#else
	addr_in->sin_family = AF_INET;
#endif
	*addrlen = sizeof *addr_in;

	return 0;
}

static int inet_getpeername(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct sockaddr_in *addr_in;

	assert(sk);
	assert(addr);
	assert(addrlen);

	if (*addrlen < sizeof *addr_in) {
		return -EINVAL;
	}

	addr_in = (struct sockaddr_in *)addr;
	memcpy(addr_in, &to_inet_sock(sk)->dst_in, sizeof *addr_in);
#if 0
	assert((addr_in->sin_family == AF_UNSPEC)
			|| (addr_in->sin_family == AF_INET));
#else
	addr_in->sin_family = AF_INET;
#endif
	*addrlen = sizeof *addr_in;

	return 0;
}

static int inet_getsockopt(struct sock *sk, int level,
		int optname, void *optval, socklen_t *optlen) {
	assert(sk);
	assert(optval);
	assert(optlen);
	assert(*optlen >= 0);

	if (level != IPPROTO_IP) {
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

static int inet_setsockopt(struct sock *sk, int level,
		int optname, const void *optval, socklen_t optlen) {
	struct inet_sock *inet;
	int val = 0;

	assert(sk);
	assert(optval);
	assert(optlen >= 0);

	inet = to_inet_sock(sk);

	if (level != IPPROTO_IP) {
		assert(sk->p_ops != NULL);
		if (sk->p_ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->p_ops->setsockopt(sk, level, optname, optval,
				optlen);
	}

	switch (optname) {
	case IP_HDRINCL:
		if (optlen <= 0 || optlen > sizeof(int)) {
			return -EFAULT;
		}
		memcpy(&val, optval, optlen);
		break;
	}

	switch (optname) {
	case IP_HDRINCL:
		if (sk->opt.so_type != SOCK_RAW) {
			return -ENOPROTOOPT;
		}
		inet->opt.hdrincl = val ? 1 : 0;
		break;
	case IP_TOS:
		break;
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

static int inet_shutdown(struct sock *sk, int how) {
	assert(sk);
	assert(sk->p_ops != NULL);

	if (sk->p_ops->shutdown == NULL) {
		return -EOPNOTSUPP;
	}

	return sk->p_ops->shutdown(sk, how);
}

POOL_DEF(inet_sock_pool, struct inet_sock, MODOPS_AMOUNT_INET_SOCK);

static const struct sock_family_ops inet_stream_ops = {
	.init        = inet_init,
	.close       = inet_close,
	.bind        = inet_bind,
	.bind_local  = inet_bind_local,
	.connect     = inet_stream_connect,
	.listen      = inet_listen,
	.accept      = inet_accept,
	.sendmsg     = inet_sendmsg,
	.recvmsg     = inet_recvmsg,
	.getsockname = inet_getsockname,
	.getpeername = inet_getpeername,
	.getsockopt  = inet_getsockopt,
	.setsockopt  = inet_setsockopt,
	.shutdown    = inet_shutdown,
	.sock_pool   = &inet_sock_pool
};

static const struct sock_family_ops inet_dgram_ops = {
	.init        = inet_init,
	.close       = inet_close,
	.bind        = inet_bind,
	.bind_local  = inet_bind_local,
	.connect     = inet_nonstream_connect,
	.listen      = inet_listen,
	.accept      = inet_accept,
	.sendmsg     = inet_sendmsg,
	.recvmsg     = inet_recvmsg,
	.getsockname = inet_getsockname,
	.getpeername = inet_getpeername,
	.getsockopt  = inet_getsockopt,
	.setsockopt  = inet_setsockopt,
	.shutdown    = inet_shutdown,
	.sock_pool   = &inet_sock_pool
};

static const struct sock_family_ops inet_raw_ops = {
	.init        = inet_init,
	.close       = inet_close,
	.bind        = inet_bind,
	.bind_local  = inet_bind_local,
	.connect     = inet_nonstream_connect,
	.listen      = inet_listen,
	.accept      = inet_accept,
	.sendmsg     = inet_sendmsg,
	.recvmsg     = inet_recvmsg,
	.getsockname = inet_getsockname,
	.getpeername = inet_getpeername,
	.getsockopt  = inet_getsockopt,
	.setsockopt  = inet_setsockopt,
	.shutdown    = inet_shutdown,
	.sock_pool   = &inet_sock_pool
};
