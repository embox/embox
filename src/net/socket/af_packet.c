/**
 * @file
 *
 * @brief PF_PACKET protocol family socket handler
 *
 * @date 30.01.2012
 * @author Anton Bondarev
 */
#include <errno.h>
#include <embox/net/pack.h>
#include <net/socket.h>
#include <net/sock.h>


EMBOX_NET_PACK(ETH_P_ALL, ip_rcv, af_packet_init);

struct packet_sock {
	/* struct sock has to be the first member of packet_sock */
	struct sock		sk;
};

static struct proto packet_proto = {
	.name	  = "PACKET",
#if 0
	.owner	  = THIS_MODULE,
#endif
	.obj_size = sizeof(struct packet_sock),
};


static int supported_sock_type(struct socket*sock) {
	switch(sock->type) {
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_PACKET:
		return 0;
	default:
		return -ESOCKTNOSUPPORT;
	}
}

static int packet_create(struct socket *sock, int protocol) {
	struct sock *sk;

	if(0 != supported_sock_type(sock)) {
		return -ESOCKTNOSUPPORT;
	}

	sk = sk_alloc(/*net,*/ PF_PACKET, 0, &packet_proto);
	if (sk == NULL) {
		return -ENOMEM;
	}
	return ENOERR;
}

/* uses for create socket */
struct net_proto_family packet_family_ops = {
	.family = PF_PACKET,
	.create = packet_create,
#if 0
	.owner = THIS_MODULE,
#endif
};

static int af_packet_init(void) {
	return sock_register(&packet_family_ops);
}

