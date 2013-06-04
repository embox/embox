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

EMBOX_NET_FAMILY(AF_INET, inet_create);

static const struct family_ops inet_dgram_ops;
static const struct family_ops inet_raw_ops;
static const struct family_ops inet_stream_ops;

/* AF_INET socket create */
struct sock * inet_create_sock(struct proto *prot,
		int type, int protocol) {
	struct sock *sk;
	struct inet_sock *inet;

	sk = sk_alloc(AF_INET, prot);
	if (sk == NULL) {
		return NULL;
	}

	sk->sk_type = type;
	sk->sk_protocol = protocol;

	inet = inet_sk(sk);
	inet->id = 0;
	inet->uc_ttl = -1; /* TODO socket setup more options  */
	inet->mc_ttl = 1;

	inet->sport_is_alloced = 0;

	// TODO it's required?
	inet->rcv_saddr = 0;
	inet->saddr = 0;
	inet->sport = 0;
	inet->daddr = 0;
	inet->dport = 0;

	return sk;
}

static int inet_create(struct socket *sock, int type, int protocol) {
	struct sock *sk;
	struct inet_sock *inet;
	const struct net_sock *nsock;
	int port;

	assert(sock != NULL);

	type = sock->type;

	if (!net_sock_support(AF_INET, type)) {
		return -EPROTOTYPE;
	}

	nsock = net_sock_lookup(AF_INET, type, protocol);
	if (nsock == NULL) {
		return -EPROTONOSUPPORT;
	}

	sk = inet_create_sock((struct proto *)nsock->options, type,
			nsock->protocol);
	if (sk == NULL) {
		return -ENOMEM;
	}

	switch (sk->sk_protocol) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		port = ip_port_get_free(sk->sk_protocol);
		if (port < 0) {
			sk_common_release(sk);
			return port;
		}
		inet = inet_sk(sk);
		inet->sport = htons((unsigned short)port);
		inet->sport_is_alloced = 1;
		break;
	}

	sock->sk = sk;
	switch (type) {
		assert(0, "socket with unknown type");
		return -EPROTOTYPE;
	case SOCK_DGRAM:
		sock->ops = &inet_dgram_ops;
		break;
	case SOCK_STREAM:
		sock->ops = &inet_stream_ops;
		break;
	case SOCK_RAW:
		sock->ops = &inet_raw_ops;
		break;
	}
	sk->sk_socket = sock;
	sk->sk_protocol = nsock->protocol;

	return ENOERR;
}

static int inet_release(struct sock *sk) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->close != NULL) {
		if (inet_sk(sk)->sport_is_alloced) {
			ip_port_unlock(sk->sk_protocol,
					ntohs(inet_sk(sk)->sport));
		}
		sk_common_release(sk);
		return 0;
	}

	sk->sk_prot->close(sk, 0);

	return 0;
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

	ret = ip_port_lock(sk->sk_protocol, ntohs(addr_in->sin_port));
	if (ret != 0) {
		return ret;
	}

	inet_sk = (struct inet_sock *)sk;
	inet_sk->rcv_saddr = addr_in->sin_addr.s_addr;
	inet_sk->sport = addr_in->sin_port;

#if 1
	return 0;
#else
	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->bind == NULL) {
		return 0;
	}

	return sk->sk_prot->bind(sk, addr, addr_len);
#endif
}

static int inet_dgram_connect(struct sock *sk,
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

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->connect == NULL) {
		return 0;
	}

	return sk->sk_prot->connect(sk, addr, addrlen);
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

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->connect == NULL) {
		return -ENOSYS;
	}

	return sk->sk_prot->connect(sk, addr, addrlen);
}

static int inet_listen(struct sock *sk, int backlog) {
	if ((sk == NULL) || (backlog < 0)) {
		return -EINVAL;
	}

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->listen == NULL) {
		return -ENOSYS;
	}

	return sk->sk_prot->listen(sk, backlog);
}

static int inet_accept(struct sock *sk, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct sock **out_sk) {
	if ((sk == NULL) || (addr == NULL) || (addrlen == NULL)
			|| (*addrlen < sizeof(struct sockaddr_in))
			|| (out_sk == NULL)) {
		return -EINVAL;
	}

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->accept == NULL) {
		return -ENOSYS;
	}

	return sk->sk_prot->accept(sk, out_sk, addr, addrlen, flags);
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

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->sendmsg == NULL) {
		return -ENOSYS;
	}

	return sk->sk_prot->sendmsg(NULL, sk, msg,
			msg->msg_iov->iov_len, flags);
}

static int inet_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags) {
	int ret;
	struct inet_sock *inet_sk;
	struct sockaddr_in *addr_in;

	if ((sk == NULL) || (msg == NULL)) {
		return -EINVAL;
	}

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->recvmsg == NULL) {
		return -ENOSYS;
	}

	ret = sk->sk_prot->recvmsg(NULL, sk, msg,
			msg->msg_iov->iov_len, flags);
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

static int inet_shutdown(struct sock *sk, int how) {
	if (sk == NULL) {
		return -EINVAL;
	}

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->shutdown == NULL) {
		return 0;
	}

	return sk->sk_prot->shutdown(sk, how);
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

static int inet_getsockopt(struct sock *sk, int level, int optname,
		void *optval, socklen_t *optlen) {
	if ((sk == NULL) || (optval == NULL) || (optlen == NULL)
			|| (*optlen < 0)) {
		return -EINVAL;
	}

	if (level != IPPROTO_IP) {
		assert(sk->sk_prot != NULL);
		if (sk->sk_prot->getsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->sk_prot->getsockopt(sk, level, optname, optval,
				optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

static int inet_setsockopt(struct sock *sk, int level, int optname,
		const void *optval, socklen_t optlen) {
	if ((sk == NULL) || (optval == NULL) || (optlen < 0)) {
		return -EINVAL;
	}

	if (level != IPPROTO_IP) {
		assert(sk->sk_prot != NULL);
		if (sk->sk_prot->setsockopt == NULL) {
			return -EOPNOTSUPP;
		}
		return sk->sk_prot->setsockopt(sk, level, optname, (char*)optval,
				optlen);
	}

	switch (optname) {
	default:
		return -ENOPROTOOPT;
	}

	return 0;
}

/**
 * predicate to compare two internet address structures
 **/
static bool inet_address_compare(struct sockaddr *addr1, struct sockaddr *addr2) {
	struct sockaddr_in *addr_in1, *addr_in2;

	addr_in1 = (struct sockaddr_in *)addr1;
	addr_in2 = (struct sockaddr_in *)addr2;
	return (addr_in1->sin_family == addr_in2->sin_family)
			&& (addr_in1->sin_addr.s_addr == addr_in2->sin_addr.s_addr)
			&& (addr_in1->sin_port == addr_in2->sin_port);
}

static const struct family_ops inet_dgram_ops = {
		.release           = inet_release,
		.bind              = inet_bind,
		.connect           = inet_dgram_connect,
		.listen            = inet_listen,
		.accept            = inet_accept,
		.sendmsg           = inet_sendmsg,
		.recvmsg           = inet_recvmsg,
		.shutdown          = inet_shutdown,
		.getsockname       = inet_getsockname,
		.getpeername       = inet_getpeername,
		.getsockopt        = inet_getsockopt,
		.setsockopt        = inet_setsockopt,
		.compare_addresses = inet_address_compare
};

static const struct family_ops inet_raw_ops = {
		.release           = inet_release,
		.bind              = inet_bind,
		.connect           = inet_dgram_connect,
		.listen            = inet_listen,
		.accept            = inet_accept,
		.sendmsg           = inet_sendmsg,
		.recvmsg           = inet_recvmsg,
		.shutdown          = inet_shutdown,
		.getsockname       = inet_getsockname,
		.getpeername       = inet_getpeername,
		.getsockopt        = inet_getsockopt,
		.setsockopt        = inet_setsockopt,
		.compare_addresses = inet_address_compare
};

static const struct family_ops inet_stream_ops = {
		.release           = inet_release,
		.bind              = inet_bind,
		.connect           = inet_stream_connect,
		.listen            = inet_listen,
		.accept            = inet_accept,
		.sendmsg           = inet_sendmsg,
		.recvmsg           = inet_recvmsg,
		.shutdown          = inet_shutdown,
		.getsockname       = inet_getsockname,
		.getpeername       = inet_getpeername,
		.getsockopt        = inet_getsockopt,
		.setsockopt        = inet_setsockopt,
		.compare_addresses = inet_address_compare
};
