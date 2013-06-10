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
#include <net/socket_registry.h>
#include <net/inet_sock.h>
#include <net/inetdevice.h>
#include <net/if_ether.h>
#include <embox/net/family.h>
#include <embox/net/sock.h>

#include <net/ip_port.h>
#include <net/inet_sock.h>

static const struct family_ops inet_dgram_ops;
static const struct family_ops inet_raw_ops;
static const struct family_ops inet_stream_ops;

static const struct net_family_type inet_types[] = {
	{ SOCK_DGRAM, &inet_dgram_ops },
	{ SOCK_RAW, &inet_raw_ops },
	{ SOCK_STREAM, &inet_stream_ops }
};

EMBOX_NET_FAMILY(AF_INET, inet_types);

static int inet_init(struct sock *sock) {
	struct inet_sock *inet_sk;
	int port;

	if (sock == NULL) {
		return -EINVAL;
	}

	inet_sk = (struct inet_sock *)sock;
	inet_sk->id = 0;
	inet_sk->uc_ttl = -1;
	inet_sk->mc_ttl = 1;
	inet_sk->sport_is_alloced = 0;
	inet_sk->rcv_saddr = 0;
	inet_sk->saddr = 0;
	inet_sk->sport = 0;
	inet_sk->daddr = 0;
	inet_sk->dport = 0;

	switch (inet_sk->sk.opt.so_protocol) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		port = ip_port_get_free(inet_sk->sk.opt.so_protocol);
		if (port < 0) {
			sock_release(&inet_sk->sk);
			return port;
		}
		inet_sk->sport = htons((unsigned short)port);
		inet_sk->sport_is_alloced = 1;
		break;
	}

	return 0;
}

static int inet_close(struct sock *sk) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->close == NULL) {
		if (inet_sk(sk)->sport_is_alloced) {
			ip_port_unlock(sk->opt.so_protocol,
					ntohs(inet_sk(sk)->sport));
		}
		sock_release(sk);
		return 0;
	}

	return sk->ops->close(sk);
}

static int inet_bind(struct sock *sk, const struct sockaddr *addr,
		socklen_t addrlen) {
	int ret;
	struct inet_sock *inet_sk;
	const struct sockaddr_in *addr_in;

	if ((sk == NULL) || (addr == NULL)
			|| (addrlen != sizeof *addr_in)) {
		return -EINVAL;
	}

	addr_in = (const struct sockaddr_in *)addr;
	if (addr_in->sin_family != AF_INET) {
		return -EINVAL;
	}
	else if ((addr_in->sin_addr.s_addr != INADDR_ANY) &&
			!ip_is_local(addr_in->sin_addr.s_addr, true, true)) {
		return -EADDRNOTAVAIL;
	}

	ret = ip_port_lock(sk->opt.so_protocol,
			ntohs(addr_in->sin_port));
	if (ret != 0) {
		return ret;
	}

	inet_sk = (struct inet_sock *)sk;
	inet_sk->rcv_saddr = addr_in->sin_addr.s_addr;
	inet_sk->sport = addr_in->sin_port;

	return 0;
}

static int inet_nonstream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	const struct sockaddr_in *addr_in;

	if ((sk == NULL) || (addr == NULL)
			|| (addrlen != sizeof *addr_in)) {
		return -EINVAL;
	}

	addr_in = (const struct sockaddr_in *)addr;
	if (addr_in->sin_family != AF_INET) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->connect == NULL) {
		return 0;
	}

	return sk->ops->connect(sk, addr, addrlen, flags);
}

static int inet_stream_connect(struct sock *sk,
		const struct sockaddr *addr, socklen_t addrlen,
		int flags) {
	const struct sockaddr_in *addr_in;

	if ((sk == NULL) || (addr == NULL)
			|| (addrlen != sizeof(struct sockaddr_in))) {
		return -EINVAL;
	}

	addr_in = (const struct sockaddr_in *)addr;
	if (addr_in->sin_family != AF_INET) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->connect == NULL) {
		return -ENOSYS;
	}

	return sk->ops->connect(sk, addr, addrlen, flags);
}

static int inet_listen(struct sock *sk, int backlog) {
	if ((sk == NULL) || (backlog < 0)) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->listen == NULL) {
		return -ENOSYS;
	}

	return sk->ops->listen(sk, backlog);
}

static int inet_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct sock **out_sk) {
	if ((sk == NULL) || (addr == NULL) || (addrlen == NULL)
			|| (*addrlen < sizeof(struct sockaddr_in))
			|| (out_sk == NULL)) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->accept == NULL) {
		return -ENOSYS;
	}

	return sk->ops->accept(sk, addr, addrlen, flags, out_sk);
}

static int inet_sendmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	struct inet_sock *inet_sk;
	const struct sockaddr_in *addr_in;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	if (msg->msg_name != NULL) {
		if (msg->msg_namelen != sizeof *addr_in) {
			return -EINVAL;
		}
		addr_in = (const struct sockaddr_in *)msg->msg_name;
		if (addr_in->sin_family != AF_INET) {
			return -EINVAL;
		}
		inet_sk = (struct inet_sock *)sk;
		inet_sk->daddr = addr_in->sin_addr.s_addr;
		inet_sk->dport = addr_in->sin_port;
	}

	assert(sk->ops != NULL);
	if (sk->ops->sendmsg == NULL) {
		return -ENOSYS;
	}

	return sk->ops->sendmsg(sk, msg, flags);
}

static int inet_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	int ret;
	struct inet_sock *inet_sk;
	struct sockaddr_in *addr_in;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->recvmsg == NULL) {
		return -ENOSYS;
	}

	ret = sk->ops->recvmsg(sk, msg, flags);
	if (ret != 0) {
		return ret;
	}

	if (msg->msg_name != NULL) {
		if (msg->msg_namelen < sizeof *addr_in) {
			return -EINVAL;
		}
		inet_sk = (struct inet_sock *)sk;
		addr_in = (struct sockaddr_in *)msg->msg_name;
		addr_in->sin_family = AF_INET;
		addr_in->sin_addr.s_addr = inet_sk->daddr;
		addr_in->sin_port = inet_sk->dport;
		msg->msg_namelen = sizeof *addr_in;
	}

	return 0;
}

static int inet_getsockname(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct inet_sock *inet_sk;
	struct sockaddr_in *addr_in;

	if ((sk == NULL) || (addr == NULL) || (addrlen == NULL)
			|| (*addrlen < sizeof *addr_in)) {
		return -EINVAL;
	}

	inet_sk = (struct inet_sock *)sk;
	addr_in = (struct sockaddr_in *)addr;
	addr_in->sin_family = AF_INET;
	addr_in->sin_addr.s_addr = inet_sk->rcv_saddr;
	addr_in->sin_port = inet_sk->sport;
	*addrlen = sizeof *addr_in;

	return 0;
}

static int inet_getpeername(struct sock *sk,
		struct sockaddr *addr, socklen_t *addrlen) {
	struct inet_sock *inet_sk;
	struct sockaddr_in *addr_in;

	if ((sk == NULL) || (addr == NULL) || (addrlen == NULL)
			|| (*addrlen < sizeof *addr_in)) {
		return -EINVAL;
	}

	inet_sk = (struct inet_sock *)sk;
	addr_in = (struct sockaddr_in *)addr;
	addr_in->sin_family = AF_INET;
	addr_in->sin_addr.s_addr = inet_sk->daddr;
	addr_in->sin_port = inet_sk->dport;
	*addrlen = sizeof *addr_in;

	return 0;
}

static int inet_getsockopt(struct sock *sk, int level,
		int optname, void *optval, socklen_t *optlen) {
	if ((sk == NULL) || (optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return -EINVAL;
	}

	if (level != IPPROTO_IP) {
		assert(sk->ops != NULL);
		if (sk->ops->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->ops->getsockopt(sk, level, optname, optval,
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
	if ((sk == NULL) || (optval == NULL) || (optlen < 0)) {
		return -EINVAL;
	}

	if (level != IPPROTO_IP) {
		assert(sk->ops != NULL);
		if (sk->ops->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->ops->setsockopt(sk, level, optname, optval,
				optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

static int inet_shutdown(struct sock *sk, int how) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->ops != NULL);
	if (sk->ops->shutdown == NULL) {
		return 0;
	}

	return sk->ops->shutdown(sk, how);
}

static bool inet_address_compare(struct sockaddr *addr1, struct sockaddr *addr2) {
	struct sockaddr_in *addr_in1, *addr_in2;

	addr_in1 = (struct sockaddr_in *)addr1;
	addr_in2 = (struct sockaddr_in *)addr2;
	return (addr_in1->sin_family == addr_in2->sin_family)
			&& (addr_in1->sin_addr.s_addr == addr_in2->sin_addr.s_addr)
			&& (addr_in1->sin_port == addr_in2->sin_port);
}

static const struct family_ops inet_dgram_ops = {
	.init              = inet_init,
	.close             = inet_close,
	.bind              = inet_bind,
	.connect           = inet_nonstream_connect,
	.listen            = inet_listen,
	.accept            = inet_accept,
	.sendmsg           = inet_sendmsg,
	.recvmsg           = inet_recvmsg,
	.getsockname       = inet_getsockname,
	.getpeername       = inet_getpeername,
	.getsockopt        = inet_getsockopt,
	.setsockopt        = inet_setsockopt,
	.shutdown          = inet_shutdown,
	.compare_addresses = inet_address_compare
};

static const struct family_ops inet_raw_ops = {
	.init              = inet_init,
	.close             = inet_close,
	.bind              = inet_bind,
	.connect           = inet_nonstream_connect,
	.listen            = inet_listen,
	.accept            = inet_accept,
	.sendmsg           = inet_sendmsg,
	.recvmsg           = inet_recvmsg,
	.getsockname       = inet_getsockname,
	.getpeername       = inet_getpeername,
	.getsockopt        = inet_getsockopt,
	.setsockopt        = inet_setsockopt,
	.shutdown          = inet_shutdown,
	.compare_addresses = inet_address_compare
};

static const struct family_ops inet_stream_ops = {
	.init              = inet_init,
	.close             = inet_close,
	.bind              = inet_bind,
	.connect           = inet_stream_connect,
	.listen            = inet_listen,
	.accept            = inet_accept,
	.sendmsg           = inet_sendmsg,
	.recvmsg           = inet_recvmsg,
	.getsockname       = inet_getsockname,
	.getpeername       = inet_getpeername,
	.getsockopt        = inet_getsockopt,
	.setsockopt        = inet_setsockopt,
	.shutdown          = inet_shutdown,
	.compare_addresses = inet_address_compare
};
