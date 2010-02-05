/**
 * @file
 *
 * @brief Implementes raw socket function
 *
 * @date 04.02.2010
 * @author Anton Bondarev
 */

#include <net/protocol.h>
#include <net/socket.h>
#include <net/sock.h>
#include <net/net.h>
#include <net/in.h>

static const struct proto raw_prot = { .name = "RAW"
#if 0
		,
		.owner = THIS_MODULE,
		.close = raw_close,
		.connect = ip4_datagram_connect,
		.disconnect = udp_disconnect,
		.ioctl = raw_ioctl,
		.init = raw_init,
		.setsockopt = raw_setsockopt,
		.getsockopt = raw_getsockopt,
		.sendmsg = raw_sendmsg,
		.recvmsg = raw_recvmsg,
		.bind = raw_bind,
		.backlog_rcv = raw_rcv_skb,
		.hash = raw_v4_hash,
		.unhash = raw_v4_unhash,
		.obj_size = sizeof(struct raw_sock),
#endif
}		;

/*
 * For SOCK_RAW sockets; should be the same as inet_dgram_ops but without
 * udp_poll
 */
static const struct proto_ops inet_sockraw_ops = { .family = PF_INET
#if 0
		.owner = THIS_MODULE,
		.release = inet_release,
		.bind = inet_bind,
		.connect = inet_dgram_connect,
		.socketpair = sock_no_socketpair,
		.accept = sock_no_accept,
		.getname = inet_getname,
		.poll = datagram_poll,
		.ioctl = inet_ioctl,
		.listen = sock_no_listen,
		.shutdown = inet_shutdown,
		.setsockopt = sock_common_setsockopt,
		.getsockopt = sock_common_getsockopt,
		.sendmsg = inet_sendmsg,
		.recvmsg = sock_common_recvmsg,
		.mmap = sock_no_mmap,
		.sendpage = inet_sendpage,
#endif
		};

static struct inet_protosw raw_socket = { .type = SOCK_RAW,
		.protocol = IPPROTO_IP, /* wild card */
		.prot = &raw_prot, .ops = &inet_sockraw_ops, .no_check = 0 /*UDP_CSUM_DEFAULT*/
};
DECLARE_INET_SOCK(raw_socket);
