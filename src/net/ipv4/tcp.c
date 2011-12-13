/**
 * @file
 * @brief Implementation of the Transmission Control Protocol (TCP).
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
#include <assert.h>

#include <hal/ipl.h>

#include <embox/unit.h>
#include <kernel/timer.h>

#define REXMIT_DELAY 1000

EMBOX_NET_PROTO_INIT(IPPROTO_TCP, tcp_v4_rcv, NULL, tcp_init);

LIST_HEAD(rexmit_socks);

static tcp_sock_t *tcp_hash[CONFIG_MAX_KERNEL_SOCKETS];

OBJALLOC_DEF(tcp_socks, struct tcp_sock, CONFIG_MAX_KERNEL_SOCKETS);

static int tcp_rexmit(struct tcp_sock *tcpsk);

static void timer_handler(sys_timer_t* timer, void *param) {
	struct list_head *resocks = (struct list_head *) param;
	struct tcp_sock *tcpsk;

	list_for_each_entry(tcpsk, resocks, rexmit_link) {
		tcp_rexmit(tcpsk);
	}
}

static int tcp_init(void) {
	sys_timer_t *timer;

	return timer_set(&timer, REXMIT_DELAY, timer_handler, (void *)&rexmit_socks);
}

static sock_t *tcp_v4_sock_alloc(void) {
	return (sock_t *) objalloc(&tcp_socks);
}

static sk_buff_t * alloc_prep_skb(int addit_len) {
	sk_buff_t *skb;

	skb = alloc_skb(ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE +
				    /*inet->opt->optlen +*/ TCP_V4_HEADER_MIN_SIZE +
				    addit_len, 0);
	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;
	skb->nh.iph->ihl = IP_MIN_HEADER_SIZE >> 2;
	skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE + addit_len);
	skb->h.raw = (unsigned char *) skb->nh.raw + IP_MIN_HEADER_SIZE;// + inet->opt->optlen;

	memset(skb->h.raw, 0, TCP_V4_HEADER_MIN_SIZE);

	return skb;
}

static void tcp_v4_sock_free(sock_t *sock) {
	list_del(&((struct tcp_sock *) sock)->rexmit_link);
	objfree(&tcp_socks, sock);
}


int tcp_v4_init_sock(sock_t *sk) {
	struct tcp_sock *tcpsk = (struct tcp_sock *) sk;
	sk->sk_state = TCP_CLOSE;
	tcpsk->conn_wait = alloc_skb_queue();

	list_add(&((struct tcp_sock *) tcpsk)->rexmit_link, &rexmit_socks);

	return 0;
}

static int tcp_v4_data_len(struct sk_buff *skb) {
	return ntohs(skb->nh.iph->tot_len) - IP_HEADER_SIZE(skb->nh.iph) - TCP_V4_HEADER_SIZE(skb->h.th);
}

static void tcp_set_st(struct tcp_sock *tcpsk, struct sk_buff *skb, char state) {
	if (skb == NULL) {
		TCP_SOCK(tcpsk)->sk_state = state;
		printf("TCP_SOCK set state %d\n", state);
	} else {
		skb->prot_info = state;
		printf("TCP_SOCK req state %d, actual %d\n", state, TCP_SOCK(tcpsk)->sk_state);
	}
}

static inline unsigned short tcp_checksum (__be32 saddr, __be32 daddr, __u8 proto,
		struct tcphdr *tcph, unsigned short size) {
	struct tcp_pseudohdr ptcph = {
		.saddr = saddr,
		.daddr = daddr,
		.zero  = 0,
		.protocol = proto,
		.tcp_len = htons(size)
	};
	return (~fold_short(partial_sum(&ptcph, sizeof(struct tcp_pseudohdr)) +
			partial_sum(tcph, size)) & 0xffff);
}

static int tcp_data_last(struct sk_buff *skb) {
	return skb->len - (skb->p_data - skb->data);
}

static void tcp_sock_queue_skb(struct tcp_sock *tcpsk, struct sk_buff *skb) {
	sock_queue_rcv_skb((struct sock *) tcpsk, skb);
	skb->p_data = skb->h.raw + TCP_V4_HEADER_SIZE(skb->h.th);

}

static inline void rebuild_tcp_header(__be32 ip_src, __be32 ip_dest,
		__be16 source, __be16 dest, __be32 seq, __be32 ack_seq,
		__be16 window,
	       	struct sk_buff *skb) {
	struct tcphdr *tcph = tcp_hdr(skb);
	tcph->source = source;
	tcph->dest = dest;
	tcph->seq = seq;
	tcph->ack_seq = ack_seq;
	tcph->doff = TCP_V4_HEADER_MIN_SIZE >> 2;
	tcph->window = window;

	tcph->check = 0;
	tcph->check = tcp_checksum(ip_src, ip_dest, IPPROTO_TCP,
		       tcph, TCP_V4_HEADER_SIZE(tcph) + tcp_v4_data_len(skb));

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
#if 0
static inline int tcp_opt_process(struct tcphdr *tcph, struct tcphdr *otcph, struct tcp_sock *tcpsk) {
	char *ptr = (char *) &tcph->options;
	for(;;) {
		switch(*ptr) {
		case TCP_OPT_KIND_EOL:
			return (int) ptr - (int) &tcph->options;
		case TCP_OPT_KIND_NOP:
			ptr++;
			break;
		case TCP_OPT_KIND_MSS:
			ptr+=2;
			tcpsk->mss = ntohs((__be16) *ptr);
		}
	}
	return 0;

#endif

/* opts are :
 *	- reliable
 */
#define SEND_OPS_RELIABLE 0x1
static int send_from_sock(struct sock *sk, sk_buff_t *skb, int ops) {
	struct inet_sock *inet = inet_sk(sk);
	struct tcp_sock *tcpsk = (struct tcp_sock *) sk;

	rebuild_tcp_header(inet->saddr, inet->daddr,
			inet->sport, inet->dport, htonl(tcpsk->this_unack), htonl(tcpsk->rem.seq),
			tcpsk->this.wind, skb);
	if (ops & SEND_OPS_RELIABLE) {
		struct sk_buff *c_skb = skb_clone(skb, 0);
		c_skb->p_data = c_skb->h.raw + TCP_V4_HEADER_SIZE(c_skb->h.th);
		printf("send_from_sock 0x%x, postponded 0x%x\n", (int) skb, (int) c_skb);
		skb_queue_tail(sk->sk_write_queue, c_skb);
	}

	printf("send_from_sock 0x%x\n", (int) skb);

	return ip_send_packet(inet, skb);
}

enum {
	TCP_ST_DROP = 1,
	TCP_ST_NO_FREE,
	TCP_ST_SEND_ONCE,
	TCP_ST_SEND_RELIABLE
};

static int tcp_st_listen(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {

	assert(TCP_SOCK(tcpsk)->sk_state == TCP_LISTEN);
	if (tcph->ack) {
		// segment RST
		//out_tcph->seq = tcph->ack_seq;
		out_tcph->rst |= 1;
		return TCP_ST_SEND_ONCE;
	}
	if (tcph->syn) {
		skb_queue_tail(tcpsk->conn_wait, skb);
		tcp_set_st(tcpsk, NULL, TCP_SYN_RECV_PRE);
		return TCP_ST_NO_FREE;
	}
	return TCP_ST_DROP;
}

static int tcp_st_drop(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
	return TCP_ST_DROP;
}

static int tcp_st_syn_recv_pre2(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
	tcpsk->this_unack = 100;
	tcpsk->this.seq = tcpsk->this_unack + 1;
	tcpsk->rem.seq = ntohl(tcph->seq) + 1;

	out_tcph->syn |= 1;
	out_tcph->ack |= 1;

	tcp_set_st(tcpsk, skb, TCP_SYN_RECV);

	return TCP_ST_SEND_RELIABLE;
}

static int tcp_st_syn_recv(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
	unsigned long seq = ntohl(tcph->seq);
	unsigned long ack = ntohl(tcph->ack_seq);

	assert(TCP_SOCK(tcpsk)->sk_state == TCP_SYN_RECV);

	if (tcpsk->rem.seq == seq) {
		//if ! tcph->rst
		//if ! tcph->syn
		if (tcph->ack)  {
			if (ack == tcpsk->this.seq) {
				struct inet_sock *inet_sk = (struct inet_sock *) tcpsk;
				tcpsk->this.seq = ack;
				tcpsk->rem.seq = seq;
				inet_sk->dport = tcph->source;
				inet_sk->sport = tcph->dest;
				inet_sk->daddr = skb->nh.iph->saddr;
				inet_sk->saddr = skb->nh.iph->daddr;
				tcp_set_st(tcpsk, NULL, TCP_ESTABIL);
			} else {
				if (tcph->fin) {
				} else {
				}
			}
		}
	} else if (tcph->rst) {
//		out_tcph->ack |= 1;
//		//CHECK about unack & ack
//		return TCP_ST_SEND_ONCE;
	}

	return TCP_ST_DROP;
}

static int process_ack_with_queue(struct tcp_sock *tcpsk, struct sk_buff *skb, int seq, int ack) {
	struct sock *sk = (struct sock *) tcpsk;
	struct sk_buff *sent_skb ;

	while ((sent_skb = skb_peek(sk->sk_write_queue))) {
		sent_skb->p_data += (ack - tcpsk->this_unack);

		printf("process_queue: skb 0x%x\n", (int) sent_skb);
		if (tcp_data_last(sent_skb) <= 0) {
			kfree_skb(sent_skb);
			printf("process_ack: freeing rexmitting skb 0x%x\n", (int) sent_skb);
			if (sent_skb->prot_info != TCP_NONE_STATE) {
				printf("TCP_SOCK set state by ack %d\n", sent_skb->prot_info);
				tcp_set_st(tcpsk, NULL, sent_skb->prot_info);
			}
		} else {
			break;
		}

	}
	printf("process_ack: done with queue\n");
	return 0;
}

static int process_rst(struct tcp_sock *tcpsk, struct sk_buff *skb, tcphdr_t *tcph) {
	struct sock *sk = (struct sock *) tcpsk;
	if (tcph->rst) {
		tcp_set_st(tcpsk, NULL, TCP_CLOSE);
		sk_common_release(sk);
		return 1;
	}
	return 0;
}

static int pre_process(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
	unsigned long seq = ntohl(tcph->seq);
	unsigned long ack = ntohl(tcph->ack_seq);

	printf("pre_process: ack %d, this_unack %d, this.seq %d\n\t seq %u, rem.seq %u\n",
			(int) ack, (int) tcpsk->this_unack,(int) tcpsk->this.seq,
			(unsigned int) seq, (unsigned int) tcpsk->rem.seq);

	if (tcpsk->this_unack <= ack && ack <= tcpsk->this.seq) {
		if (process_rst(tcpsk, skb, tcph)) {
			return TCP_ST_DROP;
		}

		process_ack_with_queue(tcpsk, skb, seq, ack);

		tcpsk->this_unack = ack;

		if (seq == tcpsk->rem.seq) {

		} else if (seq < tcpsk->rem.seq) {
			out_tcph->ack |= 1;
			printf("pre_process: dup\n");
			return TCP_ST_SEND_ONCE;
		}
	} else {

	}
	printf("pre_process: no_free\n");

	return TCP_ST_NO_FREE;

}

// return true if there was data to rexmit (data was rexmit)
static int tcp_rexmit(struct tcp_sock *tcpsk) {
	struct sock *sk = (struct sock *) tcpsk;
	//struct inet_sock *inet = inet_sk(sk);
	struct sk_buff *skb;
	int i = 0; struct list_head *l;
	//printf("tcp_rexmit: count \n ");
	list_for_each(l, (struct list_head *) sk->sk_write_queue) {
		i++;
	}
	//printf("tcp_rexmit: %d in rexmit queue\n", i);
	if ((skb = skb_dequeue(sk->sk_write_queue)) != NULL) {
		printf("!rexmited 0x%x\n", (int) skb);
		send_from_sock(sk, skb, SEND_OPS_RELIABLE);
		return (int) skb;
	}
	return 0;
}

static int tcp_st_close(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
#if 0
	unsigned long seq = ntohl(tcph->seq);
	unsigned long ack = ntohl(tcph->ack_seq);

	if (process_ack(tcpsk, skb, seq, ack) > 0) {
		//send reset ?
	}
#endif
	out_tcph->rst |= 1;
	return TCP_ST_DROP;
}
static int tcp_st_estabil(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
//	unsigned long seq = ntohl(tcph->seq);
	//unsigned long ack = ntohl(tcph->ack_seq);
	int data_len;

	assert(TCP_SOCK(tcpsk)->sk_state == TCP_ESTABIL);

	if (tcph->rst && tcph->syn && !tcph->ack) {
		return TCP_ST_DROP;
	}

	if ((data_len = tcp_v4_data_len(skb)) > 0) {
		printf("st_estabil: received %d\n", data_len);
		tcpsk->rem.seq += data_len;
		tcp_sock_queue_skb(tcpsk, skb_copy(skb, 0));

		out_tcph->ack |= 1;
		return TCP_ST_SEND_ONCE;
	}

	return TCP_ST_DROP;
}

static int tcp_st_finwait_1(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {
	out_tcph->ack |= 1; // return SEND means send ACK

	if (tcph->ack) {
		tcpsk->this.seq = 1 + ntohl(skb->h.th->ack_seq); //FIXME move from here
		if (tcph->fin) {
			tcp_set_st(tcpsk, NULL, TCP_CLOSE); // TCP_TIMEWAIT
			tcpsk->rem.seq += 1;
			out_tcph->ack |= 1;
			return TCP_ST_SEND_ONCE;
		} else {
			tcp_set_st(tcpsk, skb, TCP_FINWAIT_2);
		}

	} else if (tcph->fin) {
		tcp_set_st(tcpsk, skb, TCP_CLOSING);
		return TCP_ST_SEND_RELIABLE;
	}

	return TCP_ST_DROP;
}

static int tcp_st_finwait_2(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {

	if (tcph->ack) {
		tcpsk->rem.seq = ntohl(tcph->seq) + 1;
		out_tcph->ack |= 1;
		tcp_set_st(tcpsk, NULL, TCP_CLOSE);
		return TCP_ST_SEND_ONCE;
	}

	return TCP_ST_DROP;
}

static int tcp_st_closing(struct tcp_sock *tcpsk, struct sk_buff *skb,
		tcphdr_t *tcph, tcphdr_t *out_tcph) {

	if (tcph->ack) {
		tcp_set_st(tcpsk, NULL, TCP_CLOSE);
	}

	return TCP_ST_DROP;
}

static tcp_handler_t tcp_st_handler[TCP_MAX_STATE] = {
	[TCP_LISTEN]		= tcp_st_listen,
	[TCP_SYN_RECV]		= tcp_st_syn_recv,
	[TCP_SYN_RECV_PRE]	= tcp_st_drop,
	[TCP_SYN_RECV_PRE2]	= tcp_st_syn_recv_pre2,
	[TCP_ESTABIL]		= tcp_st_estabil,
	//[TCP_ESTABIL_ACK_WAIT]	= tcp_st_estabil_ack_wait,
	[TCP_FINWAIT_1]		= tcp_st_finwait_1,
	[TCP_FINWAIT_2]		= tcp_st_finwait_2,
	[TCP_CLOSING]		= tcp_st_closing,
	[TCP_CLOSE]		= tcp_st_close,
};

/* if < 0 then further processing cant be made */
static int tcp_handle(struct tcp_sock *tcpsk, sk_buff_t *skb, tcp_handler_t hnd) {
	char out_tcph_raw[TCP_V4_HEADER_MIN_SIZE];
	tcphdr_t *out_tcph = (tcphdr_t *) out_tcph_raw;
	int res = 0;

	memset(out_tcph, 0, TCP_V4_HEADER_MIN_SIZE);
	out_tcph->doff = TCP_V4_HEADER_MIN_SIZE >> 2;

	if ((res = hnd(tcpsk, skb, skb->h.th, out_tcph)) >= TCP_ST_SEND_ONCE) {
		uint16_t dest = skb->h.th->dest;
		uint16_t src  = skb->h.th->source;
		struct inet_sock *inet_sk = (struct inet_sock *) tcpsk;

		skb->len = ETH_HEADER_SIZE +
				IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE;

		skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE);
		memcpy(tcp_hdr(skb), out_tcph, TCP_V4_HEADER_SIZE(out_tcph));

		inet_sk->dport = src;
		inet_sk->sport = dest;
		inet_sk->daddr = skb->nh.iph->saddr;
		inet_sk->saddr = skb->nh.iph->daddr;
		send_from_sock((struct sock *) tcpsk, skb,
				(res == TCP_ST_SEND_RELIABLE ? SEND_OPS_RELIABLE : 0));

		printf("tcp_handle: send\n");
		return -1;

	} else if (res < TCP_ST_NO_FREE) {
		kfree_skb(skb);
		return -1;
	}

	return 0;
}
static int level = 0;
static void tcp_v4_process(struct tcp_sock *tcpsk, sk_buff_t *skb) {
	struct sock *sk = TCP_SOCK(tcpsk);
	printf("===== process: start %d =====\n", level++);
	if (skb->h.th->ack) {
		if (tcp_handle(tcpsk, skb, pre_process) < 0) {
//			return;
			goto leave;
		}
	}

	if (tcp_rexmit(tcpsk)) {
//		return;
//		goto leave;
	}

	printf("process: sock state = %d\n", sk->sk_state);
	tcp_handle(tcpsk, skb, tcp_st_handler[sk->sk_state]);

leave:	printf("process: done %d\n", --level);
}

static int tcp_v4_rcv(sk_buff_t *skb) {
	iphdr_t *iph = ip_hdr(skb);
	tcphdr_t *tcph = tcp_hdr(skb);
	struct tcp_sock *tcpsk = tcp_lookup(iph->daddr, tcph->dest);
	skb->links = 1;

	if (tcpsk == NULL) {
		return -1;
	}

	tcp_v4_process(tcpsk, skb);

	return 0;
}

static int tcp_v4_listen(struct sock *sk, int backlog) {
	while (sk->sk_state != TCP_CLOSE) {
	}

	sk->sk_state = TCP_LISTEN;
	((struct tcp_sock *) sk)->this.wind = htons(100);
	return 0;
}

static int tcp_v4_accept(sock_t *sk, sockaddr_t *_addr, int *addr_len) {
	sk_buff_t *skb;
	struct tcp_sock *tcpsk = (struct tcp_sock *) sk;
	while (sk->sk_state != TCP_SYN_RECV_PRE) {
	}

	if ((skb = skb_dequeue(tcpsk->conn_wait))) {
		struct sockaddr_in *addr = (struct sockaddr_in *) _addr;

		addr->sin_family = AF_INET;
		addr->sin_port = skb->h.th->source;
		addr->sin_addr.s_addr = skb->nh.iph->saddr;

		sk->sk_state = TCP_SYN_RECV_PRE2;
		tcp_v4_process(tcpsk, skb);

		while (sk->sk_state != TCP_ESTABIL && sk->sk_state != TCP_CLOSE);
	}

	return (sk->sk_state == TCP_ESTABIL ? 0 : -1);
}

static void tcp_v4_close(sock_t *sk, long timeout) {
	struct sk_buff *skb = alloc_prep_skb(0);
	tcphdr_t *tcph = tcp_hdr(skb);

	tcph->fin |= 1;
	tcph->ack |= 1;

	while (skb_peek(sk->sk_write_queue));

	printf("req: close\n");

	((struct tcp_sock *) sk)->this.seq += 1;
	tcp_set_st((struct tcp_sock *) sk, skb, TCP_FINWAIT_1);

	send_from_sock(sk, skb, SEND_OPS_RELIABLE);

	skb_queue_purge(((struct tcp_sock *) sk)-> conn_wait);

	while (sk->sk_state != TCP_CLOSE);

	sk_common_release(sk);
}

int tcp_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
	sk_buff_t *skb;
	struct tcp_sock *tcpsk = (struct tcp_sock *) sk;

	if (sk->sk_state != TCP_ESTABIL) {
		return -EINVAL;
	}

	skb = alloc_prep_skb(msg->msg_iov->iov_len);

	memcpy((void*)((unsigned int)(skb->h.raw + TCP_V4_HEADER_MIN_SIZE)),
				(void *) msg->msg_iov->iov_base, msg->msg_iov->iov_len);

	/* Fill TCP header */
	tcpsk->this_unack = tcpsk->this.seq;
	tcpsk->this.seq += len;
	skb->h.th->psh |= 1;
	skb->h.th->ack |= 1;
	printf("sendmsg: sending len %d, unack %d, seq %d\n", len, (int) tcpsk->this_unack,
			(int) tcpsk->this.seq);
#if 0
	while (sk->sk_state != TCP_ESTABIL) {
	}
#endif
	return send_from_sock(sk, skb, SEND_OPS_RELIABLE);
}

int tcp_v4_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags, int *addr_len) {
	struct sk_buff *skb = NULL;

	if (sk->sk_state != TCP_ESTABIL) {
		return -EINVAL;
	}

	do {
		skb = skb_peek_datagram(sk, flags, 0, 0);

	} while (sk->sk_state == TCP_ESTABIL && !skb);

	if (skb) {
		if (len > tcp_data_last(skb)) {
			skb = skb_recv_datagram(sk, flags, 0, 0);
			len = tcp_data_last(skb);
		}

	 	msg->msg_iov->iov_len = len;

		memcpy((void *) msg->msg_iov->iov_base, skb->p_data, len);

		skb->p_data += len;

		if (0 == tcp_data_last(skb)) {
			kfree_skb(skb);
		}

	} else {
		len = 0;
	}
	return len;
}

struct proto tcp_prot = {
	.name                   = "TCP",
	.init                   = tcp_v4_init_sock,
	.hash                   = tcp_v4_hash,
	.unhash                 = tcp_v4_unhash,
	.listen			= tcp_v4_listen,
	.accept                 = tcp_v4_accept,
	.close                  = tcp_v4_close,
	.sendmsg		= tcp_v4_sendmsg,
	.recvmsg		= tcp_v4_recvmsg,
	.sock_alloc		= tcp_v4_sock_alloc,
	.sock_free		= tcp_v4_sock_free
#if 0
	.owner                  = THIS_MODULE,
	.connect                = tcp_v4_connect,
	.disconnect             = tcp_disconnect,
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

