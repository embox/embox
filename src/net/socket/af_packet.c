/**
 * @file
 * @brief PF_PACKET protocol family socket handler
 *
 * @date 30.01.12
 * @author Anton Bondarev
 */

#include <errno.h>
#include <sys/socket.h>
#include <framework/net/sock/self.h>
#include <net/sock.h>
#include <embox/net/family.h>
#include <net/l3/ipv4/ip.h>
#include <stdlib.h>
#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_SOCKETS OPTION_GET(NUMBER, amount_sockets)

static const struct sock_family_ops packet_raw_ops;
static const struct net_family_type packet_types[] = {
	{ SOCK_STREAM, NULL },
	{ SOCK_DGRAM, NULL},
	{ SOCK_RAW, &packet_raw_ops }
};
static const struct net_pack_out_ops packet_out_ops_struct;
static const struct net_pack_out_ops *const packet_out_ops = &packet_out_ops_struct;
EMBOX_NET_FAMILY(AF_PACKET, packet_types, packet_out_ops);

static const struct sock_proto_ops packet_sock_ops_struct;
EMBOX_NET_SOCK(AF_PACKET, SOCK_RAW, 0x300 /*htons(ETH_P_ALL)*/, 0, packet_sock_ops_struct);

struct packet_sock {
	/* struct sock has to be the first member of packet_sock */
	struct sock sk;
};

static int packet_sock_init(struct sock *sk) {
	return 0;
}

POOL_DEF(packet_sock_pool, struct packet_sock, 2);
static const struct sock_family_ops packet_raw_ops = {
	.init        = packet_sock_init,
	.sock_pool   = &packet_sock_pool
};

#if 0
/* Prototypes */
static const struct family_ops packet_family_ops;
static const struct proto packet_proto;

static struct packet_sock *packet_table[MODOPS_AMOUNT_SOCKETS];

static void packet_hash(struct sock *sk) {
	size_t i;

	for (i = 0; i < sizeof packet_table / sizeof packet_table[0]; ++i) {
		if (packet_table[i] == NULL) {
			packet_table[i] = (struct packet_sock *)sk;
			break;
		}
	}
}

static void packet_unhash(struct sock *sk) {
	size_t i;

	for (i = 0; i < sizeof packet_table / sizeof packet_table[0]; ++i) {
		if (packet_table[i] == (struct packet_sock *)sk) {
			packet_table[i] = NULL;
			break;
		}
	}
}

#if 0
static int packet_sock_release(struct socket *sock) {
	sk_common_release(sock->sk);
	return 0;
}
#endif

static int supported_sock_type(struct sock *sock) {
	switch (sock->type) {
	default:
		return -ESOCKTNOSUPPORT;
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_PACKET:
		return ENOERR;
	}
}

static int packet_create(struct socket *sock, int type, int protocol) {
	int res;
	struct sock *sk;

	res = supported_sock_type(sock);
	if (res < 0) {
		return res;
	}

	sk = sk_alloc(/*net,*/ PF_PACKET, (struct proto *)&packet_proto);
	if (sk == NULL) {
		return -ENOMEM;
	}
	sock->sk = sk;
	sock->ops = &packet_family_ops;
	sk->sk_protocol = protocol;

	return ENOERR;
}

static const struct proto packet_proto = {
		.name	  = "PACKET",
		.hash	  = packet_hash,
		.unhash	  = packet_unhash,
		.obj_size = sizeof(struct packet_sock),
};
#endif
