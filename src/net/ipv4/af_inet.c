/**
 * @file af_inet.c
 *
 * @brief PF_INET protocol family socket handler.
 * @date 01.12.09
 * @author Nikolay Korotky
 */

#include <kernel/module.h>
#include <net/in.h>
#include <err.h>
#include <net/protocol.h>
#include <net/arp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/netdevice.h>

#if 0
DECLARE_INIT("net", inet_init, INIT_NET_LEVEL);
#endif

/*inet socket function*/

/*create inet socket*/
static int inet_create(struct socket *sock, int protocol) {
	struct sock *sk = sock->sk;
	sk->__sk_common.skc_prot->init(sk);
	return 0;
}

int inet_release(struct socket *sock) {
        struct sock *sk = sock->sk;
        sock->sk = NULL;
        sk->__sk_common.skc_prot->close(sk, 0);
        return 0;
}

int inet_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len) {
	struct sock *sk = sock->sk;
	sk->__sk_common.skc_prot->bind(sk, uaddr, addr_len);
	return 0;
}

int inet_dgram_connect(struct socket *sock, struct sockaddr * uaddr,
                       int addr_len, int flags) {
        struct sock *sk = sock->sk;
        sk->__sk_common.skc_prot->connect(sk, (struct sockaddr *)uaddr, addr_len);
        return 0;
}

int inet_sendmsg(/*struct kiocb *iocb,*/ struct socket *sock,/* struct msghdr *msg,*/
                 size_t size) {
        struct sock *sk = sock->sk;
        sk->__sk_common.skc_prot->sendmsg(/*iocb,*/ sk, /*msg,*/ size);
        return 0;
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
	/* Set the ARP module up */
	//arp_init();

	/* Set the IP module up */
	ip_init();

	/* Set the ICMP layer up */
	//icmp_init();

	//dev_add_pack(&ip_packet_type);
    	sock_register(&inet_family_ops);

	return 0;
}

static packet_type_t ip_packet_type = {
		.type = ETH_P_IP,
		.func = ip_rcv,
		.init = inet_init
};

DECLARE_NET_PACKET(ip_packet_type);
