/**
 * @file
 * @brief PF_PACKET protocol family socket handler
 *
 * @date 30.01.12
 * @author Anton Bondarev
 */

#include <errno.h>
#include <sys/socket.h>
#include <net/sock.h>
#include <embox/net/family.h>
#include <net/if_ether.h>
#include <net/ip.h>
#include <stdlib.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_SOCKETS OPTION_GET(NUMBER, amount_sockets)

EMBOX_NET_FAMILY(AF_PACKET, packet_create);

struct packet_sock {
	/* struct sock has to be the first member of packet_sock */
	struct sock sk;
};

/* Prototypes */
static const struct proto_ops packet_proto_ops;
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

static int supported_sock_type(struct socket *sock) {
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
	sock->ops = &packet_proto_ops;
	sk->sk_protocol = protocol;

	return ENOERR;
}

static const struct proto packet_proto = {
		.name	  = "PACKET",
		.hash	  = packet_hash,
		.unhash	  = packet_unhash,
		.obj_size = sizeof(struct packet_sock),
};
