/**
 * @file
 * @brief PF_INET protocol family socket handler.
 *
 * @date 01.12.09
 * @author Nikolay Korotky
 */

#include <err.h>
#include <net/protocol.h>
#include <net/arp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/netdevice.h>

/*inet socket function*/

/*create inet socket*/
static int inet_create(struct socket *sock, int protocol) {
	struct sock *sk;
	struct inet_sock *inet;
	int err = 0;
	extern inet_protosw_t *__ipstack_sockets_start, *__ipstack_sockets_end;
	inet_protosw_t ** p_netsock = &__ipstack_sockets_start;

	for (; p_netsock < &__ipstack_sockets_end; p_netsock++) {
		if ((* p_netsock)->type != sock->type) {
			continue;
		}
		if ((* p_netsock)->protocol == protocol) {
			if (protocol != IPPROTO_IP) {
				break;
			}
		} else {
			if (IPPROTO_IP == protocol) {
				protocol = (* p_netsock)->protocol;
				break;
			}
			if (IPPROTO_IP == (* p_netsock)->protocol) {
				break;
			}
		}
	}
	sock->ops = (* p_netsock)->ops;
	sock->sk = sk_alloc(PF_INET, 0, (struct proto*)(* p_netsock)->prot);
	sk = sock->sk;
	if (sk == NULL) {
		return -1;
	}
	inet = inet_sk(sk);
	inet->id = 0;
	inet->uc_ttl = 64;
	inet->mc_ttl = 64;
	sk->sk_type = sock->type;
	sk->sk_protocol = protocol;
	if (sk->sk_prot->init) {
		err = sk->sk_prot->init(sk);
	}
	return err;
}

int inet_release(struct socket *sock) {
	struct sock *sk = sock->sk;
	sk->sk_prot->close(sk, 0);
	sock->sk = NULL;
	return 0;
}

int inet_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len) {
	struct sockaddr_in *addr = (struct sockaddr_in *) uaddr;
	struct sock *sk = sock->sk;
	struct inet_sock *inet = inet_sk(sk);
	if (sk->sk_prot->bind) {
		sk->sk_prot->bind(sk, uaddr, addr_len);
	}
	inet->rcv_saddr = inet->saddr = addr->sin_addr.s_addr;
	inet->sport = addr->sin_port;
	inet->daddr = 0;
	inet->dport = 0;
	return 0;
}

int inet_dgram_connect(struct socket *sock, struct sockaddr * uaddr,
			int addr_len, int flags) {
	struct sock *sk = sock->sk;
	sk->sk_prot->connect(sk, (struct sockaddr *) uaddr, addr_len);
	return 0;
}

int inet_sendmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size) {
	struct sock *sk = sock->sk;
	return sk->sk_prot->sendmsg(iocb, sk, msg, size);
}

/* uses for create socket */
static struct net_proto_family inet_family_ops = {
	.family = PF_INET,
	.create = inet_create,
#if 0
	.owner = THIS_MODULE,
#endif
};

static int inet_init(void) {
	/* Init skb pool */
	sock_init();

	/* Add all the base protocols. */

	inet_protocols_init();

	/* Set the IP module up */
	ip_init();

	sock_register(&inet_family_ops);

	return 0;
}

static packet_type_t ip_packet_type = {
	.type = ETH_P_IP,
	.func = ip_rcv,
	.init = inet_init
};

DECLARE_NET_PACKET(ip_packet_type);
