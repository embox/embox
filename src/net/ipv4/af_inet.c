/**
 * @file
 * @brief PF_INET protocol family socket handler.
 *
 * @date 01.12.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <embox/net/pack.h>
#include <framework/net/sock/api.h>
//#include <framework/net/sock/types.h> // should be included
#include <stddef.h>
#include <linux/aio.h>
#include <net/protocol.h>
#include <net/in.h>
#include <net/sock.h>
#include <net/socket.h>
#include <net/inet_sock.h>

EMBOX_NET_PACK(ETH_P_IP, ip_rcv, inet_init);

static int inet_create(struct socket *sock, int protocol) {
	int res;
	struct sock *sk;
	struct inet_sock *inet;
	struct inet_protosw *p_netsock;
	const struct net_sock *net_sock_ptr;

	net_sock_foreach(net_sock_ptr) {
		p_netsock = net_sock_ptr->netsock;
		if (p_netsock->type != sock->type) {
			continue;
		}
		/* TODO modify if-else block --Ilia Vaprol */
		if (p_netsock->protocol == protocol) {
			if (protocol != IPPROTO_IP) {
				break;
			}
		} else {
			if (IPPROTO_IP == protocol) {
				protocol = p_netsock->protocol;
				break;
			}
			if (IPPROTO_IP == p_netsock->protocol) {
				break;
			}
		}
		/* TODO What if loop was completed without break? */
	}

	sk = sk_alloc(PF_INET, 0, (struct proto *)p_netsock->prot);
	if (sk == NULL) {
		return -ENOMEM;
	}

	sock->sk = sk;
	sock->ops = p_netsock->ops;
	sk->sk_type = sock->type;
	sk->sk_protocol = protocol;

	inet = inet_sk(sk);
	inet->id = 0;
	inet->uc_ttl = 64; /* TODO remove constant */
	inet->mc_ttl = 64;

	if (sk->sk_prot->init != NULL) {
		res = sk->sk_prot->init(sk);
	}

	return res;
}

void inet_release(struct socket *sock) {
	struct sock *sk;

	sk = sock->sk;
	if (sk != NULL) {
		sk->sk_prot->close(sk, 0);
		sock->sk = NULL;
	}
}

int inet_bind(struct socket *sock, struct sockaddr *addr, int addr_len) {
	int res;
	struct sock *sk;
	struct sockaddr_in *addr_in;
	struct inet_sock *inet;

	sk = sock->sk;
	if (sk->sk_prot->bind != NULL) {
		res = sk->sk_prot->bind(sk, addr, addr_len);
		if (res < 0) {
			return res;
		}
	}

	inet = inet_sk(sk);
	addr_in = (struct sockaddr_in *)addr;
	inet->rcv_saddr = inet->saddr = addr_in->sin_addr.s_addr;
	inet->sport = addr_in->sin_port;
	inet->daddr = 0;
	inet->dport = 0;

	return ENOERR;
}

int inet_dgram_connect(struct socket *sock, struct sockaddr * addr,
			int addr_len, int flags) {
	struct sock *sk;

	sk = sock->sk;
	return sk->sk_prot->connect(sk, addr, addr_len);
}

int inet_sendmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size) {
	struct sock *sk;

	sk = sock->sk;
	return sk->sk_prot->sendmsg(iocb, sk, msg, size);
}

/* uses for create socket */
struct net_proto_family inet_family_ops = {
	.family = PF_INET,
	.create = inet_create,
#if 0
	.owner = THIS_MODULE,
#endif
};

static int inet_init(void) {
	return sock_register(&inet_family_ops);;
}
