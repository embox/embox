/**
 * @file
 * @brief Implementation of the Transmission Control Protocol(TCP).
 * @details RFC 768
 *
 * @date 04.04.10
 * @author Nikolay Korotky
 * @author Anton Kozlov
 */

#include <net/tcp.h>

#include <string.h>
#include <mem/objalloc.h>
#include <net/inetdevice.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/socket.h>
#include <net/checksum.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#include <embox/net/proto.h>
#include <net/skbuff.h>
#include <errno.h>

EMBOX_NET_PROTO(IPPROTO_TCP, tcp_v4_rcv, NULL);

static tcp_sock_t *tcp_hash[CONFIG_MAX_KERNEL_SOCKETS];

OBJALLOC_DEF(tcp_socks, struct tcp_sock, CONFIG_MAX_KERNEL_SOCKETS);

static sock_t *tcp_v4_sock_alloc(void) {
	return (sock_t *) objalloc(&tcp_socks);
}

static void tcp_v4_sock_free(sock_t *sock) {
	objfree(&tcp_socks, sock);
}

int tcp_v4_init_sock(sock_t *sk) {
	sk->sk_state = TCP_CLOSE;

	return 0;
}

static int rebuild_tcp_header(sk_buff_t *skb, __be16 source,
					__be16 dest, size_t len) {
	tcphdr_t *tcph = tcp_hdr(skb);
	tcph->source = source;
	tcph->dest = dest;
	skb->len = len + TCP_V4_HEADER_MIN_SIZE;
	tcph->check = 0;
//	udph->check = ptclbsum((void *) udph, udph->len);
	return 0;
}

int tcp_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
	struct inet_sock *inet = inet_sk(sk);
	sk_buff_t *skb;
	if (sk->sk_state != TCP_ESTABIL) {
		return -EINVAL;
	}
	skb = alloc_skb(ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE +
				    /*inet->opt->optlen +*/ TCP_V4_HEADER_MIN_SIZE +
				    msg->msg_iov->iov_len, 0);
	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;
	skb->h.raw = (unsigned char *) skb->nh.raw + IP_MIN_HEADER_SIZE;// + inet->opt->optlen;
	memcpy((void*)((unsigned int)(skb->h.raw + TCP_V4_HEADER_MIN_SIZE)),
				(void *) msg->msg_iov->iov_base, msg->msg_iov->iov_len);
	/* Fill TCP header */
	rebuild_tcp_header(skb, inet->sport, inet->dport, len);
	return ip_send_packet(inet, skb);
}

int tcp_v4_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags, int *addr_len) {
	struct sk_buff *skb;

	if (sk->sk_state != TCP_ESTABIL) {
		return -EINVAL;
	}

	skb = skb_recv_datagram(sk, flags, 0, 0);
	if (skb && skb->len > 0) {
		struct tcphdr *tcph = tcp_hdr(skb);
		if (len > (skb->nh.iph->tot_len - TCP_V4_HEADER_SIZE(tcph))) {
			len = skb->nh.iph->tot_len - TCP_V4_HEADER_SIZE(tcph);
		}
		memcpy((void *) msg->msg_iov->iov_base,
				(void *) (skb->h.raw + TCP_V4_HEADER_SIZE(tcph)), len);
		kfree_skb(skb);
	} else {
		len = 0;
	}
	msg->msg_iov->iov_len = len;
	return len;
}
//TODO move to hash table routines
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
static struct tcp_sock *tcp_lookup(in_addr_t daddr, __be16 dport) {
	struct inet_sock *inet;
	size_t i;
	for (i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; i++) {
		inet = inet_sk((struct sock*) tcp_hash[i]);
		if (inet) {
			if ((inet->rcv_saddr == INADDR_ANY || inet->rcv_saddr == daddr) &&
					inet->sport == dport) {
				return (struct tcp_sock*) inet;
			}
		}
	}
	return NULL;
}
//TODO end of hash table routines

#define TCP_ST_SEND 1
#define TCP_ST_NO_SEND 1

static void tcp_set_st(struct tcp_sock *tcpsk, char state) {
	TCP_SOCK(tcpsk)->sk_state = state;
}
static int tcp_st_listen(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
	if (TCP_SOCK(tcpsk)->sk_state != TCP_LISTEN) {
		return TCP_ST_NO_SEND;
	}
	if (tcph->ack) {
		// segment RST
		out_tcph->seq = tcph->ack_seq;
		out_tcph->rst |= 1;
		return TCP_ST_SEND;
	}
	if (tcph->syn) {
		tcpsk->seq_unack = out_tcph->seq = 0;
		out_tcph->ack_seq = tcph->seq;
		out_tcph->syn |= 1;
		out_tcph->ack |= 1;

		tcpsk->seq = 1;
		tcpsk->ack_seq = tcph->seq + 1;

		tcp_set_st(tcpsk, TCP_SYN_RECV);

		return TCP_ST_SEND;
	}
	return TCP_ST_NO_SEND;
}

static int tcp_v4_listen(struct sock *sk, int backlog) {
	sk->sk_state = TCP_LISTEN;
	return 0;
}

static int (*tcp_st_handler[TCP_MAX_STATE])(struct tcp_sock *tcpsk,
		struct sk_buff *skb, tcphdr_t *tcph, tcphdr_t *out_tcph) = {
	[TCP_LISTEN] = tcp_st_listen
};

static int tcp_v4_rcv(sk_buff_t *skb) {
	iphdr_t *iph = ip_hdr(skb);
	tcphdr_t *tcph = tcp_hdr(skb);
	struct tcp_sock *sock = tcp_lookup(iph->daddr, tcph->dest);

	char out_tcph_raw[TCP_V4_HEADER_MIN_SIZE];
	tcphdr_t *out_tcph = (tcphdr_t *) out_tcph_raw;


	if (tcp_st_handler[TCP_SOCK(sock)->sk_state](sock, skb, tcph, out_tcph)
			== TCP_ST_SEND) {
		uint32_t ip_dest = skb->nh.iph->daddr;
		uint32_t ip_src  = skb->nh.iph->saddr;
		out_tcph->dest = tcph->source;
		out_tcph->source = tcph->dest;

		ip_send_reply(NULL, ip_dest, ip_src, skb, skb->len);
	}
	return 0;
}

struct proto tcp_prot = {
	.name                   = "TCP",
	.init                   = tcp_v4_init_sock,
	.hash                   = tcp_v4_hash,
	.unhash                 = tcp_v4_unhash,
	.listen			= tcp_v4_listen,
	.sendmsg		= tcp_v4_sendmsg,
	.recvmsg		= tcp_v4_recvmsg,
	.sock_alloc		= tcp_v4_sock_alloc,
	.sock_free		= tcp_v4_sock_free
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

