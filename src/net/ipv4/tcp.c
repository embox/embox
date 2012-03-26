/**
 * @file
 * @brief Implementation of the Transmission Control Protocol (TCP).
 * @details RFC 768
 *
 * @date 04.04.10
 * @author Nikolay Korotky
 * @author Anton Kozlov
 * @author Ilia Vaprol
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
#include <net/route.h>
#include <net/skbuff.h>
#include <errno.h>
#include <assert.h>

#include <hal/ipl.h>

#include <embox/unit.h>
#include <kernel/timer.h>
#include <embox/net/sock.h>


EMBOX_NET_PROTO_INIT(IPPROTO_TCP, tcp_v4_rcv, NULL, tcp_v4_init);

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, tcp_prot, inet_stream_ops, 0, true);


OBJALLOC_DEF(objalloc_tcp_socks, struct tcp_sock, CONFIG_MAX_KERNEL_SOCKETS); /* Allocator for tcp_sock structure */
static LIST_HEAD(rexmit_socks); /* List of all tcp_sock with non-empty rexmit_link list */
static struct tcp_sock *tcp_hash[CONFIG_MAX_KERNEL_SOCKETS]; /* All TCP sockets in system */

#define REXMIT_DELAY       1000 /* Delay for periodical rexmit */
#define TCP_WINDOW_DEFAULT 500  /* default for window field */
/* Send options */
#define SEND_OPS_RELIABLE  0x1  /* options for send_from_sock */

/* Error code of TCP handlers */
typedef enum tcp_st_err {
	TCP_ST_NONE = 0,     /* default state */
	TCP_ST_DROP,         /* drop packet */
	TCP_ST_NO_FREE,      /* don't free packet */
	TCP_ST_SEND_ONCE,    /* send answer */
	TCP_ST_SEND_RELIABLE /* send answer with SEND_OPS_RELIABLE (must be after TCP_ST_SEND_ONCE) */
} tcp_st_err_t;

/* Union for conversions between socket types */
union sock_pointer {
	struct sock *sk;
	struct inet_sock *inet_sk;
	struct tcp_sock *tcp_sk;
};

/* Type of TCP state handlers */
typedef tcp_st_err_t (*tcp_handler_t)(union sock_pointer sock,
		struct sk_buff *skb, struct tcphdr *tcph, struct tcphdr *out_tcph);


/************************ Debug functions ******************************/
static inline void packet_print(union sock_pointer sock, struct sk_buff *skb, char *msg,
		in_addr_t ip, uint16_t port) {
	printf("%s:%d %s sk 0x%p skb 0x%p seq %u ack %u flags %s %s %s %s %s %s %s %s\n",
			// info
			inet_ntoa(*(struct in_addr*)&ip), ntohs(port), msg, sock.tcp_sk, skb,
			// seq, ack
			ntohl(skb->h.th->seq), ntohl(skb->h.th->ack_seq),
			// flags
			(skb->h.th->ack ? "ACK" : ""), (skb->h.th->syn ? "SYN" : ""),
			(skb->h.th->fin ? "FIN" : ""), (skb->h.th->rst ? "RST" : ""),
			(skb->h.th->psh ? "PSH" : ""), (skb->h.th->urg ? "URG" : ""),
			(skb->h.th->ece ? "ECE" : ""), (skb->h.th->cwr ? "CWR" : ""));
}


/************************ Auxiliary functions **************************/
static struct sk_buff * alloc_prep_skb(unsigned int addit_len) {
	struct sk_buff *skb;

	skb = alloc_skb(ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE +
				    /*inet->opt->optlen +*/ TCP_V4_HEADER_MIN_SIZE +
				    addit_len, 0);
	if (skb != NULL) {
		skb->nh.raw = skb->data + ETH_HEADER_SIZE;
		skb->nh.iph->ihl = IP_MIN_HEADER_SIZE >> 2;
		skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE + addit_len);
		skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE;// + inet->opt->optlen;
		memset(skb->h.raw, 0, TCP_V4_HEADER_MIN_SIZE);
	}
	return skb;
}

static void tcp_sock_queue_skb(union sock_pointer sock, struct sk_buff *skb) {
	skb->p_data = skb->h.raw + TCP_V4_HEADER_SIZE(skb->h.th);
	sock_queue_rcv_skb(sock.sk, skb);
}

static unsigned int tcp_data_len(struct sk_buff *skb) {
	return ntohs(skb->nh.iph->tot_len) - IP_HEADER_SIZE(skb->nh.iph) - TCP_V4_HEADER_SIZE(skb->h.th);
}

static unsigned int tcp_data_last(struct sk_buff *skb) {
	unsigned int size;

	size = skb->p_data - skb->data;
	return (skb->len > size ? skb->len - size : 0);
}

static void tcp_set_st(union sock_pointer sock, struct sk_buff *skb, char state) {
	const char *str_state[] = { "TCP_NONE_STATE", "TCP_LISTEN", "TCP_SYN_SENT",
			"TCP_SYN_RECV_PRE", "TCP_SYN_RECV", "TCP_ESTABIL", "TCP_FINWAIT_1",
			"TCP_FINWAIT_2", "TCP_CLOSEWAIT", "TCP_CLOSING", "TCP_LASTACK",
			"TCP_TIMEWAIT", "TCP_CLOSED", "TCP_MAX_STATE"};
	if (skb == NULL) {
		sock.sk->sk_state = state;
		printf("TCP_SOCK 0x%p set state %d-%s\n", sock.tcp_sk, state, str_state[state]);
	} else {
		skb->prot_info = state;
		printf("TCP_SOCK 0x%p req state %d-%s, actual %d-%s\n", sock.tcp_sk, state, str_state[state],
				sock.sk->sk_state, str_state[sock.sk->sk_state]);
	}
}

static inline uint16_t tcp_checksum(__be32 saddr, __be32 daddr, __u8 proto,
		struct tcphdr *tcph, uint16_t size) {
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

static inline void rebuild_tcp_header(__be32 ip_src, __be32 ip_dest,
		__be16 source, __be16 dest, __be32 seq, __be32 ack_seq,
		__be16 window, struct sk_buff *skb) {
	struct tcphdr *tcph = tcp_hdr(skb);

	tcph->source = source;
	tcph->dest = dest;
	tcph->seq = htonl(seq);
	tcph->ack_seq = htonl(ack_seq);
	tcph->doff = TCP_V4_HEADER_MIN_SIZE >> 2;
	tcph->window = window;
	tcph->check = 0;
	tcph->check = tcp_checksum(ip_src, ip_dest, IPPROTO_TCP,
		       tcph, TCP_V4_HEADER_SIZE(tcph) + tcp_data_len(skb));
}

static int send_from_sock(union sock_pointer sock, struct sk_buff *skb, int ops) {
	rebuild_tcp_header(sock.inet_sk->saddr, sock.inet_sk->daddr,
			sock.inet_sk->sport, sock.inet_sk->dport,
			sock.tcp_sk->this_unack, sock.tcp_sk->rem.seq, sock.tcp_sk->this.wind, skb);
	skb->sk = sock.sk;
	if (ops & SEND_OPS_RELIABLE) {
		struct sk_buff *c_skb = skb_clone(skb, 0);
		c_skb->p_data = c_skb->h.raw + TCP_V4_HEADER_SIZE(c_skb->h.th);
		printf("send_from_sock: skb 0x%p, postponded 0x%p\n", skb, c_skb);
		skb_queue_tail(sock.sk->sk_write_queue, c_skb); // FIXME think it need append to head (not tail)
	}
	else {
		printf("send_from_sock: skb 0x%p\n", skb);
	}
	packet_print(sock, skb, "<=", sock.inet_sk->daddr, sock.inet_sk->dport);
	return ip_send_packet(sock.inet_sk, skb);
}

static int tcp_rexmit(union sock_pointer sock) {
	/* Return true if data was rexmited */
	struct sk_buff *skb;
//	int i = 0; struct list_head *l;
//	list_for_each(l, (struct list_head *) sk->sk_write_queue) {
//		i++;
//	}
//	printf("tcp_rexmit: %d in rexmit queue of TCP_SOCK 0x%x\n", i, (int)tcp_sk);
	if ((skb = skb_dequeue(sock.sk->sk_write_queue)) != NULL) { // TODO skb_peek
		printf("tcp_rexmit: rexmited 0x%p from TCP_SOCK 0x%p\n", skb, sock.tcp_sk);
		send_from_sock(sock, skb, SEND_OPS_RELIABLE);
		return 1;
	}
	return 0;
}


/************************ Handlers of TCP states ***********************/
static tcp_st_err_t tcp_st_listen(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_listen\n");
	assert(sock.sk->sk_state == TCP_LISTEN);

//	if (tcph->ack) {
//		// segment RST
//		//out_tcph->seq = tcph->ack_seq;
//		out_tcph->rst |= 1;
//		return TCP_ST_SEND_ONCE;
//	}
	if (tcph->syn) {
//		printf("\t append skb 0x%p to conn_wait of TCP_SOCK 0x%p\n", skb, sock.tcp_sk);
		skb_queue_tail(sock.tcp_sk->conn_wait, skb); // TODO check if skb in conn_wait already
		return TCP_ST_NO_FREE;
	}
	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_syn_sent(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_syn_sent\n");
	assert(sock.sk->sk_state == TCP_SYN_SENT);

	if (tcph->syn) {
		sock.tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
		out_tcph->ack |= 1;
		if (tcph->ack) {
			tcp_set_st(sock, NULL, TCP_ESTABIL);
		}
		else {
			tcp_set_st(sock, NULL, TCP_SYN_RECV);
		}
		return TCP_ST_SEND_ONCE;
	}

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_syn_recv_pre(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_syn_recv_pre\n");
	assert(sock.sk->sk_state == TCP_SYN_RECV_PRE);

	sock.tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
	out_tcph->syn |= 1;
	out_tcph->ack |= 1;
	tcp_set_st(sock, NULL, TCP_SYN_RECV);
	return TCP_ST_SEND_RELIABLE;
}

static tcp_st_err_t tcp_st_syn_recv(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	__u32 seq, ack;

	printf("call tcp_st_syn_recv\n");
	assert(sock.sk->sk_state == TCP_SYN_RECV);

	seq = ntohl(tcph->seq);
	ack = ntohl(tcph->ack_seq);
	if (tcph->ack) {// && (sock.tcp_sk->this.seq == ack) && (sock.tcp_sk->rem.seq == seq)) {
//		tcp_sk->this.seq = ack;
//		tcp_sk->rem.seq = seq;
//		inet_sk->dport = tcph->source;
//		inet_sk->sport = tcph->dest;
//		inet_sk->daddr = skb->nh.iph->saddr;
//		inet_sk->saddr = skb->nh.iph->daddr;
		tcp_set_st(sock, NULL, TCP_ESTABIL);
	}
//	if (tcph->rst) {
//		out_tcph->ack |= 1;
//		//CHECK about unack & ack
//		return TCP_ST_SEND_ONCE;
//	}

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_estabil(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	unsigned int data_len;

	printf("call tcp_st_estabil\n");
	assert(sock.sk->sk_state == TCP_ESTABIL);

//	if (tcph->rst && tcph->syn && !tcph->ack) {
//		return TCP_ST_DROP;
//	}

//	if (sock.tcp_sk->rem.seq != ntohl(tcph->seq)) {
//		return TCP_ST_DROP;
//	}
	data_len = tcp_data_len(skb);
	if (data_len > 0) {
		printf("\t received %d\n", data_len);
		sock.tcp_sk->rem.seq += data_len;
		tcp_sock_queue_skb(sock, skb_copy(skb, 0));
		out_tcph->ack |= 1;
		return TCP_ST_SEND_ONCE;
	}

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_finwait_1(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_finwait_1\n");
	assert(sock.sk->sk_state == TCP_FINWAIT_1);

	if (tcph->fin) {
		tcp_set_st(sock, NULL, TCP_CLOSED); // TCP_TIMEWAIT
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack |= 1;
		return TCP_ST_SEND_ONCE;
	}
	if (tcph->ack) {
		sock.tcp_sk->this.seq = 1 + ntohl(skb->h.th->ack_seq); //FIXME move from here
		tcp_set_st(sock, skb, TCP_FINWAIT_2);
	}

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_finwait_2(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_finwait_2\n");
	assert(sock.sk->sk_state == TCP_FINWAIT_2);

	if (tcph->ack) {
		sock.tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
		out_tcph->ack |= 1;
		tcp_set_st(sock, NULL, TCP_CLOSED);
		return TCP_ST_SEND_ONCE;
	}

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_closewait(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_closewait\n");
	assert(sock.sk->sk_state == TCP_CLOSEWAIT);

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_closing(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_closing\n");
	assert(sock.sk->sk_state == TCP_CLOSING);

	if (tcph->ack) {
		tcp_set_st(sock, NULL, TCP_CLOSED);
	}

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_lastack(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_lastack\n");
	assert(sock.sk->sk_state == TCP_LASTACK);

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_timewait(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_timewait\n");
	assert(sock.sk->sk_state == TCP_TIMEWAIT);

	return TCP_ST_DROP;
}

static tcp_st_err_t tcp_st_close(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	printf("call tcp_st_close\n");
	assert(sock.sk->sk_state == TCP_CLOSED);

#if 0
	unsigned long seq = ntohl(tcph->seq);
	unsigned long ack = ntohl(tcph->ack_seq);

	if (process_ack(tcp_sk, skb, seq, ack) > 0) {
		//send reset ?
	}
#endif
	out_tcph->rst |= 1;
	return TCP_ST_DROP;
}


/************************ Process functions ****************************/
static void process_ack(union sock_pointer sock, __u32 seq, __u32 ack) {
	struct sk_buff *sent_skb;
	unsigned long ack_bytes, curr_len;

	ack_bytes = ack - sock.tcp_sk->this_unack;
	while ((ack_bytes != 0)
			&& ((sent_skb = skb_peek(sock.sk->sk_write_queue)) != NULL)) {
		printf("process_ack: skb 0x%p\n", sent_skb);
		curr_len = ack_bytes + (sent_skb->p_data - sent_skb->data);
		if (sent_skb->len <= curr_len) {
			ack_bytes = curr_len - sent_skb->len;
			printf("process_ack: freeing rexmitting skb 0x%p\n", sent_skb);
			if (sent_skb->prot_info != TCP_NONE_STATE) {
				printf("TCP_SOCK 0x%p set state by ack %d\n", sock.tcp_sk, sent_skb->prot_info);
				tcp_set_st(sock, NULL, sent_skb->prot_info);
			}
			kfree_skb(sent_skb); /* list_del will done at kfree_skb */
		}
		else {
			sent_skb->p_data += ack_bytes;
			break;
		}
	}
	printf("process_ack: done with queue\n");
}

/*static*/ int process_rst(union sock_pointer sock, struct sk_buff *skb, tcphdr_t *tcph) { // TODO static functinon
	if (tcph->rst) {
		tcp_set_st(sock, NULL, TCP_CLOSED);
		sk_common_release(sock.sk);
		return 1;
	}
	return 0;
}

static tcp_st_err_t pre_process(union sock_pointer sock, struct sk_buff *skb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	__u32 seq, ack;

	seq = ntohl(tcph->seq);
	ack = ntohl(tcph->ack_seq);
	printf("pre_process: ack %u, this_unack %u, this.seq %u, seq %u, rem.seq %u\n",
			ack, sock.tcp_sk->this_unack, sock.tcp_sk->this.seq, seq, sock.tcp_sk->rem.seq);
	if ((sock.tcp_sk->this_unack < ack) && (ack <= sock.tcp_sk->this.seq)) { // TODO another check
//		if (process_rst(tcp_sk, skb, tcph)) {
//			return TCP_ST_DROP;
//		}

		process_ack(sock, seq, ack);
		sock.tcp_sk->this_unack = ack;

		/* send segment with ack flag if this packet is duplicated */
		if (seq < sock.tcp_sk->rem.seq) {
			out_tcph->ack |= 1;
			printf("pre_process: dup\n");
			return TCP_ST_SEND_ONCE;
		}
	} else {

	}
	printf("pre_process: no_free\n");

	return TCP_ST_NO_FREE;

}

#if 0
static inline int tcp_opt_process(struct tcphdr *tcph, struct tcphdr *otcph, struct tcp_sock *tcp_sk) {
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
			tcp_sk->mss = ntohs((__be16) *ptr);
		}
	}
	return 0;

#endif


/************************ Handlers table *******************************/
static tcp_handler_t tcp_st_handler[TCP_MAX_STATE] = {
	[TCP_LISTEN]		= tcp_st_listen,
	[TCP_SYN_SENT]		= tcp_st_syn_sent,
	[TCP_SYN_RECV_PRE]	= tcp_st_syn_recv_pre,
	[TCP_SYN_RECV]		= tcp_st_syn_recv,
	[TCP_ESTABIL]		= tcp_st_estabil,
	[TCP_FINWAIT_1]		= tcp_st_finwait_1,
	[TCP_FINWAIT_2]		= tcp_st_finwait_2,
	[TCP_CLOSEWAIT]		= tcp_st_closewait,
	[TCP_CLOSING]		= tcp_st_closing,
	[TCP_LASTACK]		= tcp_st_lastack,
	[TCP_TIMEWAIT]		= tcp_st_timewait,
	[TCP_CLOSED]		= tcp_st_close,
};

static int tcp_handle(union sock_pointer sock, struct sk_buff *skb, tcp_handler_t hnd) {
	/* If result is not TCP_ST_NO_FREE then further processing can't be made */
	char out_tcph_raw[TCP_V4_HEADER_MIN_SIZE];
	struct tcphdr *out_tcph;
	uint16_t dest, src;
	int res;

	out_tcph = (struct tcphdr *)out_tcph_raw;
	memset(out_tcph, 0, TCP_V4_HEADER_MIN_SIZE);
	out_tcph->doff = TCP_V4_HEADER_MIN_SIZE >> 2;

	res = hnd(sock, skb, skb->h.th, out_tcph);
	if (res >= TCP_ST_SEND_ONCE) {
		dest = skb->h.th->dest;
		src = skb->h.th->source;

		skb->len = ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE;
		skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE);

		memcpy(tcp_hdr(skb), out_tcph, TCP_V4_HEADER_SIZE(out_tcph));

		sock.inet_sk->dport = src;
		sock.inet_sk->sport = dest;
		sock.inet_sk->daddr = skb->nh.iph->saddr;
		sock.inet_sk->saddr = skb->nh.iph->daddr;
		send_from_sock(sock, skb,
				(res == TCP_ST_SEND_RELIABLE ? SEND_OPS_RELIABLE : 0));
		printf("tcp_handle: send\n");
	} else if (res == TCP_ST_DROP) {
		kfree_skb(skb);
	}

	return res;
}

static struct tcp_sock * tcp_lookup(in_addr_t saddr, __be16 sport, in_addr_t daddr, __be16 dport) {
	size_t i;
	struct inet_sock *inet_sk;

	/* lookup socket with strict addressing */
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		inet_sk = (struct inet_sock *)tcp_hash[i];
		if (inet_sk != NULL) {
			if ((inet_sk->rcv_saddr == saddr) && (inet_sk->sport == sport)
					&& (inet_sk->daddr == daddr) && (inet_sk->dport == dport)) {
				return (struct tcp_sock *)inet_sk;
			}
		}
	}

	/* lookup another sockets */
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		inet_sk = (struct inet_sock *)tcp_hash[i];
		if (inet_sk != NULL) {
			if (((inet_sk->rcv_saddr == INADDR_ANY) || (inet_sk->rcv_saddr == saddr))
					&& (inet_sk->sport == sport)) {
				return (struct tcp_sock *)inet_sk;
			}
		}
	}

	return NULL;
}

/*
 * Main function of TCP protocol
 */
static void tcp_process(union sock_pointer sock, struct sk_buff *skb) {
	packet_print(sock, skb, "=>", skb->nh.iph->saddr, skb->h.th->source);
	if (skb->h.th->ack
			&& (tcp_handle(sock, skb, pre_process) != TCP_ST_NO_FREE)) {
		return;
	}

	if (tcp_rexmit(sock)) {
		kfree_skb(skb);
	}

	assert(tcp_st_handler[sock.sk->sk_state] != NULL);
	tcp_handle(sock, skb, tcp_st_handler[sock.sk->sk_state]);
}

static int tcp_v4_rcv(struct sk_buff *skb) {
	iphdr_t *iph;
	tcphdr_t *tcph;
	union sock_pointer sock;
	printf("\nPACKET_RCV\n");

	iph = ip_hdr(skb);
	tcph = tcp_hdr(skb);
	sock.tcp_sk = tcp_lookup(iph->daddr, tcph->dest, iph->saddr, tcph->source);
	if (sock.tcp_sk == NULL) {
		return -1;
	}

	tcp_process(sock, skb);

	return 0;
}

/*static*/ void timer_handler(sys_timer_t* timer, void *param) {
	struct list_head *resocks = (struct list_head *) param;
	union sock_pointer sock;

	list_for_each_entry(sock.tcp_sk, resocks, rexmit_link) {
		tcp_rexmit(sock);
	}
}

static int tcp_v4_init(void) {
//	sys_timer_t *timer;

//	return timer_set(&timer, REXMIT_DELAY, timer_handler, (void *)&rexmit_socks);
	return 0;
}

/************************ Socket's functions ***************************/
static void tcp_v4_close(struct sock *sk, long timeout) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;

	sock.sk = sk;
	if (sock.sk->sk_state == TCP_ESTABIL) {
		skb = alloc_prep_skb(0);
		tcph = tcp_hdr(skb);
		tcph->fin |= 1;
		tcph->ack |= 1;
		sock.tcp_sk->this.seq += 1;
		tcp_set_st(sock, NULL, TCP_FINWAIT_1);
		send_from_sock(sock, skb, SEND_OPS_RELIABLE);
	}
	else if (sock.sk->sk_state == TCP_LISTEN) {
		skb_queue_purge(sock.tcp_sk->conn_wait);
		tcp_set_st(sock, NULL, TCP_CLOSED);
	}
}

static int tcp_v4_connect(struct sock *sk, struct sockaddr *addr, int addr_len) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;
	struct sockaddr_in *addr_in;

	if (addr_len != sizeof(struct sockaddr_in)) {
		return -EINVAL;
	}
	sock.sk = sk;
	addr_in = (struct sockaddr_in *)addr;
	if (sock.sk->sk_state == TCP_CLOSED) {
		sock.inet_sk->saddr = in_dev_get(rt_fib_get_best(addr_in->sin_addr.s_addr)->dev)->ifa_address; // TODO remove this
		sock.inet_sk->daddr = addr_in->sin_addr.s_addr;
		sock.inet_sk->dport = addr_in->sin_port;
		skb = alloc_prep_skb(0);
		tcph = tcp_hdr(skb);
		tcph->syn |= 1;
		tcp_set_st(sock, NULL, TCP_SYN_SENT);
		send_from_sock(sock, skb, SEND_OPS_RELIABLE);
		while ((sk->sk_state != TCP_ESTABIL) && (sk->sk_state != TCP_CLOSED));
		return (sk->sk_state == TCP_ESTABIL ? ENOERR : -1);
	}
	return -EINVAL;
}

static int tcp_v4_listen(struct sock *sk, int backlog) {
	union sock_pointer sock;

	sock.sk = sk;
	tcp_set_st(sock, NULL, TCP_LISTEN);
	return ENOERR;
}

static int tcp_v4_accept(struct sock *sk, struct sock *newsk,
		struct sockaddr *addr, int *addr_len) {
	struct sk_buff *skb;
	union sock_pointer sock, newsock;
	struct sockaddr_in *addr_in;

	if (sk->sk_state != TCP_LISTEN) {
		printf("tcp_v4_accept: socket state is not TCP_LISTEN\n");
		return -EBADF;
	}

	sock.sk = sk;
	newsock.sk = newsk;
	addr_in = (struct sockaddr_in *)addr;
	do {
		/* waiting for clients */
		while ((skb = skb_dequeue(sock.tcp_sk->conn_wait)) == NULL);
		/* save remote address */
		addr_in->sin_family = AF_INET;
		addr_in->sin_port = skb->h.th->source;
		addr_in->sin_addr.s_addr = skb->nh.iph->saddr;
		printf("tcp_v4_accept: new TCP_SOCK 0x%x for %s:%d\n", (int)newsk,
				inet_ntoa(*(struct in_addr *)&skb->nh.iph->saddr), (int)skb->h.th->source);
		/* set up new socket */
		newsock.inet_sk->rcv_saddr = skb->nh.iph->daddr;
		newsock.inet_sk->sport = skb->h.th->dest;
		newsock.inet_sk->daddr = skb->nh.iph->saddr;
		newsock.inet_sk->dport = skb->h.th->source;
		/* processing of skb */
		tcp_set_st(newsock, NULL, TCP_SYN_RECV_PRE);
		tcp_process(newsock, skb);
		/* wait until something happens */
		while ((newsock.sk->sk_state != TCP_ESTABIL) && (newsock.sk->sk_state != TCP_CLOSED));
	} while (newsock.sk->sk_state != TCP_ESTABIL); /* repeat until new connection is not established */
	*addr_len = sizeof(struct sockaddr_in);

	return ENOERR;
}

static int tcp_v4_init_sock(struct sock *sk) {
	union sock_pointer sock;

	sock.sk = sk;
	tcp_set_st(sock, NULL, TCP_CLOSED);
	sock.tcp_sk->conn_wait = alloc_skb_queue();
	sock.tcp_sk->this_unack = 100; // TODO remove constant
	sock.tcp_sk->this.seq = sock.tcp_sk->this_unack + 1;
	sock.tcp_sk->this.wind = htons(TCP_WINDOW_DEFAULT);
	list_add(&sock.tcp_sk->rexmit_link, &rexmit_socks);

	return ENOERR;
}

int tcp_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
	sk_buff_t *skb;
	union sock_pointer sock;

	if (sk->sk_state != TCP_ESTABIL) {
		return -EINVAL;
	}

	skb = alloc_prep_skb(msg->msg_iov->iov_len);
	if (skb == NULL) {
		return -ENOMEM;
	}

	memcpy((void*)(skb->h.raw + TCP_V4_HEADER_MIN_SIZE),
				(void *)msg->msg_iov->iov_base, msg->msg_iov->iov_len);

	/* Fill TCP header */
	sock.sk = sk;
//	sock.tcp_sk->this_unack = sock.tcp_sk->this.seq;
	sock.tcp_sk->this.seq += len;
	skb->h.th->psh |= 1;
//	skb->h.th->ack |= 1;
	printf("sendmsg: sending len %d, unack %u, seq %u\n", len,
			sock.tcp_sk->this_unack, sock.tcp_sk->this.seq);
	return send_from_sock(sock, skb, SEND_OPS_RELIABLE);
}

int tcp_v4_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags, int *addr_len) {
	struct sk_buff *skb;

	do {
		if (sk->sk_state != TCP_ESTABIL) {
			return -EINVAL;
		}
	} while ((skb = skb_peek_datagram(sk, flags, 0, 0)) == NULL);

	if (len >= tcp_data_last(skb)) {
		skb_recv_datagram(sk, flags, 0, 0);
		len = tcp_data_last(skb);
	}
	msg->msg_iov->iov_len = len;
	memcpy((void *)msg->msg_iov->iov_base, skb->p_data, len);
	kfree_skb(skb);
//	skb->p_data += len;
//	if (tcp_data_last(skb) == 0) {
//			kfree_skb(skb);
//	}
	return ENOERR;
}

//TODO move to hash table routines
static void tcp_v4_hash(struct sock *sk) {
	size_t i;

//	printf("tcp_v4_hash: TCP_SOCK 0x%p\n", sk);
	for (i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		if (tcp_hash[i] == NULL) {
			tcp_hash[i] = (struct tcp_sock *)sk;
			break;
		}
	}
}

static void tcp_v4_unhash(struct sock *sk) {
	size_t i;

//	printf("tcp_v4_unhash: TCP_SOCK 0x%p\n", sk);
	for (i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		if (tcp_hash[i] == (struct tcp_sock *)sk) {
			tcp_hash[i] = NULL;
			break;
		}
	}
}

static struct sock * tcp_v4_sock_alloc(void) {
	return (struct sock *)objalloc(&objalloc_tcp_socks);
}

static void tcp_v4_sock_free(struct sock *sock) {
	list_del(&((struct tcp_sock *)sock)->rexmit_link);
	objfree(&objalloc_tcp_socks, sock);
}

////////////////////////////////////

void * get_tcp_sockets(void) {
	return (void *)tcp_hash;
}

////////////////////////////////////

struct proto tcp_prot = {
		.name       = "TCP",
		.close      = tcp_v4_close,
		.connect    = tcp_v4_connect,
		.listen     = tcp_v4_listen,
		.accept     = tcp_v4_accept,
		.init       = tcp_v4_init_sock,
//		.setsockopt = tcp_v4_setsockopt,
//		.getsockopt = tcp_v4_getsockopt,
		.sendmsg    = tcp_v4_sendmsg,
		.recvmsg    = tcp_v4_recvmsg,
		.hash       = tcp_v4_hash,
		.unhash     = tcp_v4_unhash,
		.sock_alloc = tcp_v4_sock_alloc,
		.sock_free  = tcp_v4_sock_free,
		.obj_size   = sizeof(struct tcp_sock),
};
