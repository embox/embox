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
#include <linux/aio.h>
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

static const struct proto_ops inet_raw_ops;
static const struct proto_ops inet_stream_ops;
static const struct proto_ops inet_dgram_ops;

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

static int inet_release(struct socket *sock) {
	struct sock *sk;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);

	if (sk->sk_prot->close != NULL) {
		/* altering close() interface to return NULL
			 is probably not appropriate) */
		sk->sk_prot->close(sk, 0);
	}
	else {
		if (inet_sk(sk)->sport_is_alloced) {
			ip_port_unlock(sk->sk_protocol, ntohs(inet_sk(sk)->sport));
		}
		sk_common_release(sk);
	}

	sock->sk = NULL;

	sock_unlock(sk);

	return ENOERR;
}

static int inet_bind(struct socket *sock, const struct sockaddr *addr, socklen_t addr_len) {
	int res;
	struct sock *sk;
	struct sockaddr_in *addr_in;
	struct inet_sock *inet;

	if ((addr == NULL) || (addr_len != sizeof *addr_in)) {
		return -EINVAL;
	}
	addr_in = (struct sockaddr_in *)addr;

	/* check if there is such an ip thought our local inet devices */
		/* check broadcast and multicast, is that correct? */
		/* svv: It depends. Some sockets might work with broadcast/multicast
		 * packets, some doesn't. IMHO, there is no reason for tcp to use
		 * not unicast addresses
		 */
	if (!ip_is_local(addr_in->sin_addr.s_addr, true, true)) {
		return -EADDRNOTAVAIL;
	}

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);

	inet = inet_sk(sk);
	inet->rcv_saddr = addr_in->sin_addr.s_addr;
	if (addr_in->sin_port != 0) {
		res = ip_port_lock(sk->sk_protocol, ntohs(addr_in->sin_port));
		if (res < 0) {
			goto exit_with_error;
		}
		ip_port_unlock(sk->sk_protocol, ntohs(inet->sport));
		inet->sport = addr_in->sin_port;
	}

	assert(sk->sk_prot != NULL);
	if (sk->sk_prot->bind != NULL) {
		res = sk->sk_prot->bind(sk, addr, addr_len);
		if (res < 0) {
			goto exit_with_error;
		}
	}

	sock_unlock(sock->sk);

	return ENOERR;
exit_with_error:
	sock_unlock(sock->sk);
	return res;
}

static int inet_stream_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addr_len, int flags) {
	int res;
	struct sock *sk;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->connect != NULL) {
		res = sk->sk_prot->connect(sk, addr, addr_len);
	}
	else {
		res = -EOPNOTSUPP;
	}

	sock_unlock(sk);

	return res;
}

static int inet_dgram_connect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addr_len, int flags) {
	int res;
	struct sock *sk;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->connect == NULL) {
		res = sk->sk_prot->connect(sk, addr, addr_len);
	}
	else {
		res = 0;
	}

	sock_unlock(sk);

	return res;
}

int inet_sendmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *msg, size_t size, int flags) {
	int res;
	struct sock *sk;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->sendmsg != NULL) {
		res = sk->sk_prot->sendmsg(iocb, sk, msg, size, flags);
	}
	else {
		res = -EOPNOTSUPP;
	}

	sock_unlock(sk);

	return res;
}

int inet_recvmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *msg, size_t size, int flags) {
	int res;
	struct sock *sk;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->recvmsg != NULL) {
		res = sk->sk_prot->recvmsg(iocb, sk, msg, size, flags);
	}
	else {
		res = -EOPNOTSUPP;
	}

	sock_unlock(sk);

	return res;
}

int inet_listen(struct socket *sock, int backlog) {
	int res;
	struct sock *sk;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->listen != NULL) {
		res = sk->sk_prot->listen(sk, backlog);
	}
	else {
		res = -EOPNOTSUPP;
	}

	sock_unlock(sk);

	return res;
}

static int inet_accept(struct sock *sk, struct sock **newsk, sockaddr_t *addr, int *addr_len, int flags) {
	int res;

	if (!sock_lock(&sk)) {
		return -EINVAL;
	}

	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->accept != NULL) {
		res = sk->sk_prot->accept(sk, newsk, addr, addr_len, flags);
	}
	else {
		res = -EOPNOTSUPP;
	}

	sock_unlock(sk);

	return res;
}

static int inet_setsockopt(struct socket *sock, int level, int optname,
		char *optval, int optlen) {
	struct net_device *dev;
	struct socket_opt_state *ops;
	struct sock *sk;
	int res;

	assert(sock != NULL);
	assert(sock->socket_node != NULL);

	if (optname == SO_BINDTODEVICE) {
		dev = netdev_get_by_name(optval);
		if (!dev)
			return -ENOENT;
		ops = &sock->socket_node->options;
		ops->so_bindtodev = dev;
	}

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);
	assert(sk->sk_prot->setsockopt != NULL);

	res = sk->sk_prot->setsockopt(sk, level, optname, optval, optlen);
	sock_unlock(sk);

	return res;
}

int inet_shutdown(struct socket *sock, int how) {
	struct sock *sk;
	int res;

	assert(sock != NULL);

	if (!sock_lock(&sock->sk)) {
		return -EINVAL;
	}

	sk = sock->sk;
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->shutdown != NULL) {
		res = sk->sk_prot->shutdown(sk, how);
	}
	else {
		res = 0;
	}

	return res;
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

static const struct proto_ops inet_dgram_ops = {
		.family            = PF_INET,
		.release           = inet_release,
		.bind              = inet_bind,
		.connect           = inet_dgram_connect,
		.sendmsg           = inet_sendmsg,
		.recvmsg           = inet_recvmsg,
		.compare_addresses = inet_address_compare,
		.setsockopt        = inet_setsockopt,
};

static const struct proto_ops inet_stream_ops = {
		.family            = PF_INET,
		.release           = inet_release,
		.bind              = inet_bind,
		.connect           = inet_stream_connect,
		.accept            = inet_accept,
		.listen            = inet_listen,
		.sendmsg           = inet_sendmsg,
		.recvmsg           = inet_recvmsg,
		.compare_addresses = inet_address_compare,
		.shutdown          = inet_shutdown
};

/*
 * For SOCK_RAW sockets; should be the same as inet_dgram_ops but without
 * udp_poll
 */
static const struct proto_ops inet_raw_ops = {
		.family            = PF_INET,
		.release           = inet_release,
		.bind              = inet_bind,
		.connect           = inet_dgram_connect,
		.sendmsg           = inet_sendmsg,
		.recvmsg           = inet_recvmsg,
		.compare_addresses = inet_address_compare,
};
