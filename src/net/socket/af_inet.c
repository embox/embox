/**
 * @file
 * @brief PF_INET protocol family socket handler.
 *
 * @date 01.12.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/net/sock/api.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <net/sock.h>
#include <sys/socket.h>
#include <net/socket/inet_sock.h>
#include <net/inetdevice.h>
#include <embox/net/family.h>
#include <net/sock_port.h>
#include <string.h>
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
	in_sk->src_port_alloced = 0;
	memset(&in_sk->src_in, 0, sizeof in_sk->src_in);
	memset(&in_sk->dst_in, 0, sizeof in_sk->dst_in);
	in_sk->src_in.sin_family = in_sk->dst_in.sin_family = AF_INET;

#if 0
	in_sk->sk.src_addr = &in_sk->src_in;
	in_sk->sk.dst_addr = &in_sk->dst_in;
	in_sk->sk.addr_len = sizeof(struct sockaddr_in);
#endif

	return 0;
}

static int inet_close(struct sock *sk) {
	assert(sk);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->close == NULL) {
		if (to_inet_sock(sk)->src_port_alloced) {
			assert(to_inet_sock(sk)->src_in.sin_family == AF_INET);
			sock_port_unlock(sk->p_ops->sock_port,
					ntohs(to_inet_sock(sk)->src_in.sin_port));
		}
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

static int inet_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	const struct sockaddr_in *addr_in;

	assert(sk);
	assert(addr);

	if (addrlen != sizeof *addr_in) {
		return -EINVAL;
	}

	addr_in = (const struct sockaddr_in *)addr;
	if (addr_in->sin_family != AF_INET) {
		return -EAFNOSUPPORT;
	}
	else if ((addr_in->sin_addr.s_addr != htonl(INADDR_ANY)) &&
			!ip_is_local(addr_in->sin_addr.s_addr, true, true)) {
		return -EADDRNOTAVAIL;
	}

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sock_port != NULL) {
		if (!sock_port_lock(sk->p_ops->sock_port,
					ntohs(addr_in->sin_port))) {
			return -EADDRINUSE;
		}
		to_inet_sock(sk)->src_port_alloced = 1;
	}

	__inet_bind(to_inet_sock(sk), addr_in);

	return 0;
}

static int inet_bind_local(struct sock *sk) {
	size_t port;
	struct sockaddr_in addr_in;

	assert(sk);

	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	assert(sk->p_ops != NULL);
	if (sk->p_ops->sock_port != NULL) {
		port = sock_port_alloc(sk->p_ops->sock_port);
		if (port == SOCK_PORT_NONE) {
			return -ENOMEM;
		}
		to_inet_sock(sk)->src_port_alloced = 1;
		addr_in.sin_port = htons(port);
	}
	else {
		addr_in.sin_port = 0;
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

	ret = rt_fib_source_ip(addr_in->sin_addr.s_addr, &src_ip);
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
	assert(addr_in->sin_family == AF_INET);
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
	assert(addr_in->sin_family == AF_INET);
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
	assert(sk);
	assert(optval);
	assert(optlen >= 0);

	if (level != IPPROTO_IP) {
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
