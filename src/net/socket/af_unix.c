/**
 * @file
 *
 * @brief PF_UNIX protocol family socket handler
 *
 * @date 31.01.2012
 * @author Anton Bondarev
 */
#include <errno.h>
#include <net/socket.h>
#include <net/sock.h>


static const struct proto_ops unix_stream_ops = {
	.family =	PF_UNIX,
#if 0
	.owner =	THIS_MODULE,
	.release =	unix_release,
	.bind =		unix_bind,
	.connect =	unix_stream_connect,
	.socketpair =	unix_socketpair,
	.accept =	unix_accept,
	.getname =	unix_getname,
	.poll =		unix_poll,
	.ioctl =	unix_ioctl,
	.listen =	unix_listen,
	.shutdown =	unix_shutdown,
	.setsockopt =	sock_no_setsockopt,
	.getsockopt =	sock_no_getsockopt,
	.sendmsg =	unix_stream_sendmsg,
	.recvmsg =	unix_stream_recvmsg,
	.mmap =		sock_no_mmap,
	.sendpage =	sock_no_sendpage,
#endif
};

static const struct proto_ops unix_dgram_ops = {
	.family =	PF_UNIX,
#if 0
	.owner =	THIS_MODULE,
	.release =	unix_release,
	.bind =		unix_bind,
	.connect =	unix_dgram_connect,
	.socketpair =	unix_socketpair,
	.accept =	sock_no_accept,
	.getname =	unix_getname,
	.poll =		unix_dgram_poll,
	.ioctl =	unix_ioctl,
	.listen =	sock_no_listen,
	.shutdown =	unix_shutdown,
	.setsockopt =	sock_no_setsockopt,
	.getsockopt =	sock_no_getsockopt,
	.sendmsg =	unix_dgram_sendmsg,
	.recvmsg =	unix_dgram_recvmsg,
	.mmap =		sock_no_mmap,
	.sendpage =	sock_no_sendpage,
#endif
};

static const struct proto_ops unix_seqpacket_ops = {
	.family =	PF_UNIX,
#if 0
	.owner =	THIS_MODULE,
	.release =	unix_release,
	.bind =		unix_bind,
	.connect =	unix_stream_connect,
	.socketpair =	unix_socketpair,
	.accept =	unix_accept,
	.getname =	unix_getname,
	.poll =		unix_dgram_poll,
	.ioctl =	unix_ioctl,
	.listen =	unix_listen,
	.shutdown =	unix_shutdown,
	.setsockopt =	sock_no_setsockopt,
	.getsockopt =	sock_no_getsockopt,
	.sendmsg =	unix_seqpacket_sendmsg,
	.recvmsg =	unix_dgram_recvmsg,
	.mmap =		sock_no_mmap,
	.sendpage =	sock_no_sendpage,
#endif
};

struct unix_sock {
	/* sk has to be the first member */
	struct sock sk;
};

static struct proto unix_proto = {
	.name = "UNIX",
#if 0
	.owner = THIS_MODULE,
#endif
	.obj_size = sizeof(struct unix_sock),
};

static int supported_sock_type(struct socket*sock) {
	switch (sock->type) {
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

	default:
		return -ESOCKTNOSUPPORT;
	}

	return ENOERR;
}

static int unix_create(struct socket *sock, int protocol) {
	struct sock *sk;

	if(0 != supported_sock_type(sock)) {
		return -ESOCKTNOSUPPORT;
	}

	sk = sk_alloc(/*net,*/PF_UNIX, 0, &unix_proto);
	if (sk == NULL) {
		return -ENOMEM;
	}

	return ENOERR;
}

static const struct net_proto_family unix_family_ops = {
	.family = PF_UNIX,
	.create = unix_create,
#if 0
	.owner = THIS_MODULE,
#endif
};

