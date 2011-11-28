/**
 * @file
 * @brief Implementation of the Transmission Control Protocol(TCP).
 * @details RFC 768
 *
 * @date 04.04.10
 * @author Nikolay Korotky
 */

#include <net/tcp.h>

#include <string.h>
#include <net/inetdevice.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/socket.h>
#include <net/checksum.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#include <embox/net/proto.h>
#include <err.h>

EMBOX_NET_PROTO(IPPROTO_TCP, tcp_v4_rcv, NULL);

static tcp_sock_t *tcp_hash[CONFIG_MAX_KERNEL_SOCKETS];

static int tcp_v4_rcv(sk_buff_t *skb) {
#if 0
	struct sock *sk;
	struct inet_sock *inet;
	sk_buff_t *skb_tmp;

	iphdr_t *iph = ip_hdr(skb);
	tcphdr_t *tcph = tcp_hdr(skb);
#endif
	return 0;
}

int tcp_v4_init_sock(sock_t *sk) {
	sk->sk_state = TCP_CLOSE;

	return 0;
}

static void tcp_v4_hash(struct sock *sk) {
	size_t i;
	for (i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if (tcp_hash[i] == NULL) {
			tcp_hash[i] = (tcp_sock_t *) sk;
			break;
		}
	}
}

static void tcp_v4_unhash(struct sock *sk) {
	size_t i;
	for (i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if (tcp_hash[i] == (tcp_sock_t *) sk) {
			tcp_hash[i] = NULL;
			break;
		}
	}
}

struct proto tcp_prot = {
	.name                   = "TCP",
	.init                   = tcp_v4_init_sock,
	.hash                   = tcp_v4_hash,
	.unhash                 = tcp_v4_unhash,
#if 0
	.owner                  = THIS_MODULE,
	.close                  = tcp_close,
	.connect                = tcp_v4_connect,
	.disconnect             = tcp_disconnect,
	.accept                 = inet_csk_accept,
	.ioctl                  = tcp_ioctl,
	.destroy                = tcp_v4_destroy_sock,
	.shutdown               = tcp_shutdown,
	.setsockopt             = tcp_setsockopt,
	.getsockopt             = tcp_getsockopt,
	.recvmsg                = tcp_recvmsg,
	.backlog_rcv            = tcp_v4_do_rcv,
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

