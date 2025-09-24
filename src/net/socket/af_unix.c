/**
 * @file
 *
 * @brief PF_UNIX protocol family socket handler
 *
 * @date 31.01.2012
 * @author Anton Bondarev
 */
#include <errno.h>
#include <sys/socket.h>

#include <net/sock.h>
#include <net/family.h>

EMBOX_NET_FAMILY(AF_UNIX, unix_create);

/* Prototypes */
static const struct family_ops unix_dgram_ops;
static const struct family_ops unix_stream_ops;
static const struct family_ops unix_seqpacket_ops;
static const struct proto unix_proto;

struct unix_sock {
	/* sk has to be the first member */
	struct sock sk;
};

static int supported_sock_type(struct socket *sock) {
	switch (sock->type) {
	default:
		return -ESOCKTNOSUPPORT;
	case SOCK_STREAM:
		sock->ops = &unix_stream_ops;
		break;
	case SOCK_RAW:
		sock->type = SOCK_DGRAM;
	case SOCK_DGRAM:
		sock->ops = &unix_dgram_ops;
		break;
	case SOCK_SEQPACKET:
		sock->ops = &unix_seqpacket_ops;
		break;
	}

	return ENOERR;
}

static int unix_create(struct socket *sock, int type, int protocol) {
	int res;
	struct sock *sk;

	res = supported_sock_type(sock);
	if (res < 0) {
		return res;
	}

	sk = sk_alloc(/*net,*/PF_UNIX, (struct proto *)&unix_proto);
	if (sk == NULL) {
		return -ENOMEM;
	}

	return ENOERR;
}

static const struct family_ops unix_dgram_ops = {
		.family =	PF_UNIX,
};

static const struct family_ops unix_stream_ops = {
		.family =	PF_UNIX,
};

static const struct family_ops unix_seqpacket_ops = {
		.family =	PF_UNIX,
};

static const struct proto unix_proto = {
		.name = "UNIX",
		.obj_size = sizeof(struct unix_sock),
};
