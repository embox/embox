/**
 * @file
 * @brief Implementation of the Transmission Control Protocol(TCP).
 * @details RFC 768
 *
 * @date 04.04.10
 * @author Nikolay Korotky
 */

#include <string.h>
#include <net/inetdevice.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/socket.h>
#include <net/checksum.h>
#include <net/protocol.h>
#include <net/inet_common.h>

int tcp_v4_rcv(sk_buff_t *skb) {
	printf("stub: receive tcp packet\n");
	return 0;
}

static const net_protocol_t tcp_protocol = {
	.handler = tcp_v4_rcv,
	.type = IPPROTO_TCP
};

DECLARE_INET_PROTO(tcp_protocol);

struct proto tcp_prot = {
	.name                   = "TCP",
#if 0
	.owner                  = THIS_MODULE,
	.close                  = tcp_close,
	.connect                = tcp_v4_connect,
	.disconnect             = tcp_disconnect,
	.accept                 = inet_csk_accept,
	.ioctl                  = tcp_ioctl,
	.init                   = tcp_v4_init_sock,
	.destroy                = tcp_v4_destroy_sock,
	.shutdown               = tcp_shutdown,
	.setsockopt             = tcp_setsockopt,
	.getsockopt             = tcp_getsockopt,
	.recvmsg                = tcp_recvmsg,
	.backlog_rcv            = tcp_v4_do_rcv,
	.hash                   = inet_hash,
	.unhash                 = inet_unhash,
	.get_port               = inet_csk_get_port,
	.enter_memory_pressure  = tcp_enter_memory_pressure,
	.sockets_allocated      = &tcp_sockets_allocated,
	.orphan_count           = &tcp_orphan_count,
	.memory_allocated       = &tcp_memory_allocated,
	.memory_pressure        = &tcp_memory_pressure,
	.sysctl_mem             = sysctl_tcp_mem,
	.sysctl_wmem            = sysctl_tcp_wmem,
	.sysctl_rmem            = sysctl_tcp_rmem,
	.max_header             = MAX_TCP_HEADER,
	.obj_size               = sizeof(struct tcp_sock),
	.slab_flags             = SLAB_DESTROY_BY_RCU,
	.twsk_prot              = &tcp_timewait_sock_ops,
	.rsk_prot               = &tcp_request_sock_ops,
	.h.hashinfo             = &tcp_hashinfo,
#endif
};

const struct proto_ops inet_stream_ops = {
	.family            = PF_INET,
#if 0
	.owner             = THIS_MODULE,
	.release           = inet_release,
	.bind              = inet_bind,
	.connect           = inet_stream_connect,
	.socketpair        = sock_no_socketpair,
	.accept            = inet_accept,
	.getname           = inet_getname,
	.poll              = tcp_poll,
	.ioctl             = inet_ioctl,
	.listen            = inet_listen,
	.shutdown          = inet_shutdown,
	.setsockopt        = sock_common_setsockopt,
	.getsockopt        = sock_common_getsockopt,
	.sendmsg           = tcp_sendmsg,
	.recvmsg           = sock_common_recvmsg,
	.mmap              = sock_no_mmap,
	.sendpage          = tcp_sendpage,
	.splice_read       = tcp_splice_read,
#endif
};

static struct inet_protosw tcp_socket = {
	.type = SOCK_STREAM,
	.protocol = IPPROTO_TCP,
	.prot = &tcp_prot,
	.ops = &inet_stream_ops,
	.no_check = 0,
};

DECLARE_INET_SOCK(tcp_socket);
