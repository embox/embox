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
#include <net/skbuff.h>
#include <embox/net/proto.h>
#include <net/route.h>
#include <net/skbuff.h>
#include <errno.h>
#include <err.h>
#include <assert.h>
#include <net/sock.h>

#include <hal/ipl.h>

#include <embox/unit.h>
#include <kernel/timer.h>
#include <embox/net/sock.h>
#include <kernel/softirq_lock.h>


EMBOX_NET_PROTO_INIT(IPPROTO_TCP, tcp_v4_rcv, NULL, tcp_v4_init);

EMBOX_NET_SOCK(AF_INET, SOCK_STREAM, IPPROTO_TCP, tcp_prot, inet_stream_ops, 0, true);


/** TODO
 * +1. Create default socket for resetting
 * +2. PSH flag
 * +3. RST flag
 * 4. Changes state's logic (i.e. TCP_CLOSED for all socket which doesn't exists etc.)
 * +5. Rewrite send_from_sock (don't send new skb if queue is not empty)
 * 6. tcp_sock_free in tcp_st_finwait_2
 * 7. Remove seq_queue (use rem.seq instead, build packet, and then rebuild only)
 * 8. Add lock/unlock
 */


#define REXMIT_DELAY       3000 /* Delay for periodical rexmit */
#define TCP_WINDOW_DEFAULT 500  /* default for window field */

/* Error code of TCP handlers */
enum {
	TCP_RET_OK = 1, /* all ok, don't free packet */
	TCP_RET_DROP,   /* drop packet */
	TCP_RET_SEND,   /* send answer */
	TCP_RET_ACKN    /* send acknowledgment */
};

/* Union for conversions between socket types */
union sock_pointer {
	struct sock *sk;
	struct inet_sock *inet_sk;
	struct tcp_sock *tcp_sk;
};

/* Type of TCP state handlers */
typedef int (*tcp_handler_t)(union sock_pointer sock,
		struct sk_buff **skb, struct tcphdr *tcph, struct tcphdr *out_tcph);


OBJALLOC_DEF(objalloc_tcp_socks, struct tcp_sock, CONFIG_MAX_KERNEL_SOCKETS); /* Allocator for tcp_sock structure */
static LIST_HEAD(rexmit_socks); /* List of all tcp_sock with non-empty rexmit_link list */
static struct tcp_sock *tcp_hash[CONFIG_MAX_KERNEL_SOCKETS]; /* All TCP sockets in system */
static union sock_pointer tcp_default; /* Default socket for TCP protocol. */
struct proto tcp_prot; /* prototype */

/* internet protocol stack private method for socket allocation */
extern struct sock *inet_create_sock(gfp_t priority, struct proto *prot, int protocol, int type);


/************************ Debug functions ******************************/
static inline void debug_print(__u8 code, const char *msg, ...) {
	va_list args;

	va_start(args, msg);
	if (code) {// & 0b10111111) {
		/* 0bit - warnings
		 * 1bit - tcp_handle
		 * 2bit - tcp global functions (init, send, recv etc.)
		 * 3bit - tcp state's handlers
		 * 4bit - tcp_sock_xmit, send_from_sock, send_ack_from_sock, free_rexmitting_queue,  tcp_rexmit
		 * 5bit - socket state
		 * 6bit - sock_lock / sock_unlock
		 * 7bit - packet_print
		 */
		softirq_lock();
		vprintf(msg, args);
		softirq_unlock();
	}
	va_end(args);
}

static __u32 tcp_seq_len(struct sk_buff *skb);
static inline void packet_print(union sock_pointer sock, struct sk_buff *skb, char *msg,
		in_addr_t ip, uint16_t port) {
	debug_print(128, "%s:%d %s sk 0x%p skb 0x%p seq %u ack %u seq_len %u flags %s %s %s %s %s %s %s %s\n",
			// info
			inet_ntoa(*(struct in_addr*)&ip), ntohs(port), msg, sock.tcp_sk, skb,
			// seq, ack, seq_len
			ntohl(skb->h.th->seq), ntohl(skb->h.th->ack_seq), tcp_seq_len(skb),
			// flags
			(skb->h.th->ack ? "ACK" : ""), (skb->h.th->syn ? "SYN" : ""),
			(skb->h.th->fin ? "FIN" : ""), (skb->h.th->rst ? "RST" : ""),
			(skb->h.th->psh ? "PSH" : ""), (skb->h.th->urg ? "URG" : ""),
			(skb->h.th->ece ? "ECE" : ""), (skb->h.th->cwr ? "CWR" : ""));
}


/************************ Auxiliary functions **************************/
static struct sk_buff * alloc_prep_skb(size_t addit_len) {
	struct sk_buff *skb;

	skb = alloc_skb(ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE +
				    /*inet->opt->optlen +*/ TCP_V4_HEADER_MIN_SIZE +
				    addit_len, 0);
	if (skb == NULL) {
		LOG_ERROR("no memory or len is too big\n");
		return NULL;
	}

	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;
	skb->nh.iph->ihl = IP_MIN_HEADER_SIZE / 4;
	skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE + addit_len);
	skb->h.raw = skb->nh.raw + IP_MIN_HEADER_SIZE;// + inet->opt->optlen;
	memset(skb->h.raw, 0, TCP_V4_HEADER_MIN_SIZE);
	skb->h.th->doff = TCP_V4_HEADER_MIN_SIZE / 4;

	return skb;
}

static void tcp_sock_save_skb(union sock_pointer sock, struct sk_buff *skb) {
	__u32 seq, rem_seq;

	seq = ntohl(skb->h.th->seq);
	rem_seq = sock.tcp_sk->rem.seq;
	/* setup p_data */
	assert(rem_seq >= seq); /* FIXME */
	skb->p_data = skb->h.raw + TCP_V4_HEADER_SIZE(skb->h.th) + (rem_seq - seq);
	/* move skb to socket received queue */
	sock_queue_rcv_skb(sock.sk, skb);
}

static void tcp_sock_lock(union sock_pointer sock) {
	__u8 locked;
	debug_print(64, "tcp_sock_lock: try sk 0x%p\n", sock.tcp_sk);
try_lock:
	softirq_lock();
	locked = sock.tcp_sk->lock, sock.tcp_sk->lock = 1;
	softirq_unlock();
	if (!locked) {
		goto lock_done;
	}
	goto try_lock;
lock_done:
	debug_print(64, "tcp_sock_lock: sk 0x%p locked\n", sock.tcp_sk);
}

static void tcp_sock_unlock(union sock_pointer sock) {
	debug_print(64, "tcp_sock_unlock: sk 0x%p unlocked\n", sock.tcp_sk);
	sock.tcp_sk->lock = 0;
}

static size_t tcp_data_len(struct sk_buff *skb) {
	return ntohs(skb->nh.iph->tot_len) - IP_HEADER_SIZE(skb->nh.iph) - TCP_V4_HEADER_SIZE(skb->h.th);
}

static size_t tcp_data_left(struct sk_buff *skb) {
	size_t size;

	size = skb->p_data - skb->mac.raw;
	return (skb->len > size ? skb->len - size : 0);
}

static int tcp_seq_flags(struct tcphdr *tcph) {
	return (tcph->fin || tcph->syn);
}

static size_t tcp_seq_len(struct sk_buff *skb) {
	size_t data_len;

	data_len = tcp_data_len(skb);
	return (data_len > 0 ? data_len : (size_t)tcp_seq_flags(skb->h.th));
}

static size_t tcp_seq_left(struct sk_buff *skb) {
	size_t data_left;

	data_left = tcp_data_left(skb);
	return (data_left > 0 ? data_left : (size_t)tcp_seq_flags(skb->h.th));
}

static void tcp_set_st(union sock_pointer sock, unsigned char state) {
	const char *str_state[TCP_MAX_STATE] = {"TCP_CLOSED", "TCP_LISTEN",
			"TCP_SYN_SENT", "TCP_SYN_RECV_PRE", "TCP_SYN_RECV", "TCP_ESTABIL",
			"TCP_FINWAIT_1", "TCP_FINWAIT_2", "TCP_CLOSEWAIT", "TCP_CLOSING",
			"TCP_LASTACK", "TCP_TIMEWAIT"};
	switch (state) {
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_FINWAIT_1:
	case TCP_CLOSING:
	case TCP_LASTACK:
		sock.tcp_sk->ack_flag = sock.tcp_sk->seq_queue;
		debug_print(32, "sk 0x%p set ack_flag %u for state %d-%s\n",
				sock.tcp_sk, sock.tcp_sk->ack_flag, state, str_state[state]);
		break;
	}
	sock.sk->sk_state = state;
	debug_print(32, "sk 0x%p set state %d-%s\n", sock.tcp_sk, state, str_state[state]);
}

static __u16 tcp_checksum(__be32 saddr, __be32 daddr, __u8 proto,
		struct tcphdr *tcph, __u16 size) {
	struct tcp_pseudohdr ptcph;

	ptcph.saddr = saddr;
	ptcph.daddr = daddr;
	ptcph.zero = 0;
	ptcph.protocol = proto;
	ptcph.tcp_len = htons(size);
	return (~fold_short(partial_sum(&ptcph, sizeof ptcph) +
			partial_sum(tcph, size)) & 0xffff);
}

static void rebuild_tcp_header(__be32 ip_src, __be32 ip_dest,
		__be16 source, __be16 dest, __be32 seq, __be32 ack_seq,
		__be16 window, struct sk_buff *skb) {
	struct tcphdr *tcph;

	tcph = tcp_hdr(skb);
	tcph->source = source;
	tcph->dest = dest;
	tcph->seq = htonl(seq);
	tcph->ack_seq = htonl(ack_seq);
	tcph->doff = TCP_V4_HEADER_MIN_SIZE / 4;
	tcph->window = htons(window);
	tcph->check = 0;
	tcph->check = tcp_checksum(ip_src, ip_dest, IPPROTO_TCP,
		       tcph, TCP_V4_HEADER_SIZE(tcph) + tcp_data_len(skb));
}

static int tcp_xmit(union sock_pointer sock, struct sk_buff *skb) {
	skb->sk = sock.sk; // check it
	sock.tcp_sk->this.seq = sock.tcp_sk->this_unack + tcp_seq_len(skb);
	rebuild_tcp_header(sock.inet_sk->saddr, sock.inet_sk->daddr,
			sock.inet_sk->sport, sock.inet_sk->dport,
			sock.tcp_sk->this_unack, sock.tcp_sk->rem.seq,
			sock.tcp_sk->this.wind, skb);
	packet_print(sock, skb, "<=", sock.inet_sk->daddr, sock.inet_sk->dport);
	return ip_send_packet(sock.inet_sk, skb);
}

static int tcp_sock_xmit(union sock_pointer sock) {
	struct sk_buff *skb, *skb_send;
	size_t seq_len;

	tcp_sock_lock(sock);

	/* get next skb for sending */
	skb = skb_peek(sock.sk->sk_write_queue);
	if (skb == NULL) {
		assert(sock.tcp_sk->this_unack == sock.tcp_sk->this.seq);
		assert(sock.tcp_sk->this.seq == sock.tcp_sk->seq_queue);
		tcp_sock_unlock(sock);
		return ENOERR;
	}
	seq_len = tcp_seq_len(skb);
	if (seq_len > 0) {
		skb_send = skb_clone(skb, 0);
		if (skb_send == NULL) {
			LOG_ERROR("no memory\n");
			tcp_sock_unlock(sock);
			return -ENOMEM;
		}
		debug_print(16, "tcp_sock_xmit: send skb 0x%p, postponed 0x%p\n", skb_send, skb);
	}
	else {
		skb_send = skb_dequeue(sock.sk->sk_write_queue);
		assert(skb_send == skb);
		debug_print(16, "tcp_sock_xmit: send skb 0x%p\n", skb_send);
	}

	tcp_sock_unlock(sock);

	return tcp_xmit(sock, skb_send);
}

/**
 * Send a acknowledgment, only
 */
static int send_ack_from_sock(union sock_pointer sock, struct sk_buff *skb_ackn) {
	struct sk_buff *skb;

	skb = skb_peek(sock.sk->sk_write_queue);
	if (skb == NULL) {
		/* send skb_ackn if no skb in outgoing queue */
		debug_print(16, "send_ack_from_sock: send 0x%p\n", skb_ackn);
		return tcp_xmit(sock, skb_ackn);
	}

	/* If there, set ack flag and free skb_ackn */
	skb->h.th->ack = 1;
	debug_print(16, "send_ack_from_sock: add ack to 0x%p\n", skb);
	kfree_skb(skb_ackn);

	tcp_sock_xmit(sock);

	return ENOERR;
}

/**
 * Send package
 */
static int send_from_sock(union sock_pointer sock, struct sk_buff *skb_send) {
	/* save skb */
	skb_send->p_data = skb_send->h.raw + TCP_V4_HEADER_SIZE(skb_send->h.th); // check it
//	sock.tcp_sk->seq_queue += tcp_seq_len(skb_send);
	skb_queue_tail(sock.sk->sk_write_queue, skb_send);
	debug_print(16, "send_from_sock: save 0x%p to outgoing queue\n", skb_send);
	/* send packet */
	tcp_sock_xmit(sock);
	return ENOERR;
}

static void free_rexmitting_queue(union sock_pointer sock, __u32 ack, __u32 unack) {
	struct sk_buff *sent_skb;
	size_t ack_len, seq_left;

	tcp_sock_lock(sock);

	ack_len = ack - unack;
	while ((ack_len != 0)
			&& ((sent_skb = skb_peek(sock.sk->sk_write_queue)) != NULL)) {
		seq_left = tcp_seq_left(sent_skb);
		if (seq_left <= ack_len) {
			ack_len -= seq_left;
			debug_print(16, "free_rexmitting_queue: remove skb 0x%p\n", sent_skb);
			kfree_skb(sent_skb); /* list_del will done at kfree_skb */
		}
		else {
			sent_skb->p_data += ack_len;
			break;
		}
	}

	tcp_sock_unlock(sock);
}

static int tcp_rexmit(union sock_pointer sock) {
	struct sk_buff *skb;

	skb = skb_peek(sock.sk->sk_write_queue);
	if (skb == NULL) {
		return ENOERR;
	}
	debug_print(16, "tcp_rexmit: needed rexmit skb 0x%p from sk 0x%p\n", skb_peek(sock.sk->sk_write_queue), sock.tcp_sk);

	return tcp_sock_xmit(sock);
}

static void tcp_free_sock(union sock_pointer sock) {
	skb_queue_purge(sock.tcp_sk->conn_wait);
	list_del(&sock.tcp_sk->rexmit_link);
	sk_common_release(sock.sk);
}

/************************ Handlers of TCP states ***********************/
static int tcp_st_closed(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_closed\n");
	assert(sock.sk->sk_state == TCP_CLOSED);

	out_tcph->rst = 1;
//	out_tcph->ack = 1;
	/* Set seq and ack */
	if (tcph->ack) {
		sock.tcp_sk->this_unack = ntohl(tcph->ack_seq);
		sock.tcp_sk->rem.seq = 0;
	}
	else {
		sock.tcp_sk->this_unack = 0;
		sock.tcp_sk->rem.seq = ntohl(tcph->seq) + tcp_seq_len(*pskb);
	}

	/* Set up a socket */
	sock.inet_sk->saddr = (*pskb)->nh.iph->daddr;
	sock.inet_sk->sport = tcph->dest;
	sock.inet_sk->daddr = (*pskb)->nh.iph->saddr;
	sock.inet_sk->dport = tcph->source;

	return TCP_RET_SEND;
}

static int tcp_st_listen(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_listen\n");
	assert(sock.sk->sk_state == TCP_LISTEN);

	if (tcph->syn) {
		debug_print(8, "\t append skb 0x%p to conn_wait of sk 0x%p\n", *pskb, sock.tcp_sk);
		skb_queue_tail(sock.tcp_sk->conn_wait, *pskb); // TODO check if skb in conn_wait already
		return TCP_RET_OK;
	}
	return TCP_RET_DROP;
}

static int tcp_st_syn_sent(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_syn_sent\n");
	assert(sock.sk->sk_state == TCP_SYN_SENT);

	if (tcph->syn) {
		sock.tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
//		sock.tcp_sk->rem.wind = ntohs(tcph->window);
		out_tcph->ack = 1;
		if (tcph->ack) {
			tcp_set_st(sock, TCP_ESTABIL);
		}
		else {
			tcp_set_st(sock, TCP_SYN_RECV);
		}
		return TCP_RET_ACKN;
	}

	return TCP_RET_DROP;
}

static int tcp_st_syn_recv_pre(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_syn_recv_pre\n");
	assert(sock.sk->sk_state == TCP_SYN_RECV_PRE);

	if (tcph->syn) {
		sock.tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
//		sock.tcp_sk->rem.wind = ntohs(tcph->window);
		out_tcph->ack = 1;
		out_tcph->syn = 1;
		sock.tcp_sk->seq_queue += tcp_seq_len(*pskb);
		tcp_set_st(sock, TCP_SYN_RECV);
		return TCP_RET_SEND;
	}
	return TCP_RET_DROP;
}

static int tcp_st_syn_recv(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_syn_recv\n");
	assert(sock.sk->sk_state == TCP_SYN_RECV);

	if (tcph->ack) {
		tcp_set_st(sock, TCP_ESTABIL);
	}

	return TCP_RET_DROP;
}

static int tcp_st_estabil(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	size_t data_len;
	struct sk_buff *answer;

	debug_print(8, "call tcp_st_estabil\n");
	assert(sock.sk->sk_state == TCP_ESTABIL);

	data_len = tcp_data_len(*pskb);
	if (data_len > 0) {
		/* Allocate new sk_buff_t for sending ack's flag */
		answer = alloc_prep_skb(0);
		if (answer == NULL) {
			return -ENOMEM;
		}
		/* Save current sk_buff_t with data */
		debug_print(8, "\t received %d\n", data_len);
		tcp_sock_save_skb(sock, *pskb);
		sock.tcp_sk->rem.seq += data_len;
		out_tcph->ack = 1;
		if (tcph->fin) {
			tcp_set_st(sock, TCP_CLOSEWAIT);
		}
		*pskb = answer;
		return TCP_RET_ACKN;
	}
	else if (tcph->fin) {
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack = 1;
		tcp_set_st(sock, TCP_CLOSEWAIT);
		return TCP_RET_ACKN;
	}

	return TCP_RET_DROP;
}

static int tcp_st_finwait_1(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	size_t data_len;
	struct sk_buff *answer;

	debug_print(8, "call tcp_st_finwait_1\n");
	assert(sock.sk->sk_state == TCP_FINWAIT_1);

	data_len = tcp_data_len(*pskb);
	if (data_len > 0) {
		/* Allocate new sk_buff_t for sending ack's flag */
		answer = alloc_prep_skb(0);
		if (answer == NULL) {
			return -ENOMEM;
		}
		/* Save current sk_buff_t with data */
		debug_print(8, "\t received %d\n", data_len);
		tcp_sock_save_skb(sock, *pskb);
		sock.tcp_sk->rem.seq += data_len;
		out_tcph->ack = 1;
		if (tcph->fin) {
			if (tcph->ack) {
				tcp_set_st(sock, TCP_CLOSED); /* TODO TCP_TIMEWAIT */
//				tcp_free_sock(sock); // TODO move to tcp_st_timewait
			}
			else {
				sock.tcp_sk->ack_flag = sock.tcp_sk->seq_queue;
				tcp_set_st(sock, TCP_CLOSING);
			}
		}
		else if (tcph->ack) {
			tcp_set_st(sock, TCP_FINWAIT_2);
		}
		*pskb = answer;
		return TCP_RET_ACKN;
	}
	else if (tcph->fin) {
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack = 1;
		if (tcph->ack) {
			tcp_set_st(sock, TCP_CLOSED); /* TODO TCP_TIMEWAIT */
//			tcp_free_sock(sock); // TODO move to tcp_st_timewait
		}
		else {
			tcp_set_st(sock, TCP_CLOSING);
		}
		return TCP_RET_ACKN;
	}
	else if (tcph->ack) {
		tcp_set_st(sock, TCP_FINWAIT_2);
	}

	return TCP_RET_DROP;
}

static int tcp_st_finwait_2(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	size_t data_len;
	struct sk_buff *answer;

	debug_print(8, "call tcp_st_finwait_2\n");
	assert(sock.sk->sk_state == TCP_FINWAIT_2);

	data_len = tcp_data_len(*pskb);
	if (data_len > 0) {
		/* Allocate new sk_buff_t for sending ack's flag */
		answer = alloc_prep_skb(0);
		if (answer == NULL) {
			return -ENOMEM;
		}
		/* Save current sk_buff_t with data */
		debug_print(8, "\t received %d\n", data_len);
		tcp_sock_save_skb(sock, *pskb);
		sock.tcp_sk->rem.seq += data_len;
		out_tcph->ack = 1;
		if (tcph->fin) {
			tcp_set_st(sock, TCP_CLOSED); /* TODO TCP_TIMEWAIT */
//			tcp_free_sock(sock); // TODO move to tcp_st_timewait
		}
		*pskb = answer;
		return TCP_RET_ACKN;
	}
	else if (tcph->fin) {
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack = 1;
		tcp_set_st(sock, TCP_CLOSED); /* TODO TCP_TIMEWAIT */
//		tcp_free_sock(sock); // TODO move to tcp_st_timewait
		return TCP_RET_ACKN;
	}

	return TCP_RET_DROP;
}

static int tcp_st_closewait(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_closewait\n");
	assert(sock.sk->sk_state == TCP_CLOSEWAIT);

	return TCP_RET_DROP;
}

static int tcp_st_closing(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_closing\n");
	assert(sock.sk->sk_state == TCP_CLOSING);

	if (tcph->ack) {
		tcp_set_st(sock, TCP_CLOSED); /* TODO TCP_TIMEWAIT */
		tcp_free_sock(sock); // TODO move to tcp_st_timewait
	}

	return TCP_RET_DROP;
}

static int tcp_st_lastack(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_lastack\n");
	assert(sock.sk->sk_state == TCP_LASTACK);

	if (tcph->ack) {
		tcp_set_st(sock, TCP_CLOSED);
		tcp_free_sock(sock);
	}

	return TCP_RET_DROP;
}

static int tcp_st_timewait(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_timewait\n");
	assert(sock.sk->sk_state == TCP_TIMEWAIT);

	// timeout 2msl and set TCP_CLOSED state

	return TCP_RET_DROP;
}


/************************ Process functions ****************************/
static int process_rst(union sock_pointer sock, struct tcphdr *tcph,
		struct tcphdr *out_tcph) {
	__u8 state;

	state = sock.sk->sk_state;
	switch (state) {
	default:
		if (state == TCP_SYN_SENT) {
			if (sock.tcp_sk->this.seq == ntohl(tcph->ack_seq)) {// TODO take this.seq from array
				tcp_set_st(sock, TCP_CLOSED);
			}
		}
		else {
			tcp_set_st(sock, TCP_CLOSED);
		}
		break;
	case TCP_CLOSED:
	case TCP_LISTEN:
	case TCP_SYN_RECV_PRE:
		break;
	}

	return TCP_RET_DROP;
}

static int process_ack(union sock_pointer sock, struct tcphdr *tcph,
		struct tcphdr *out_tcph) {
	__u32 ack, this_seq, this_unack;

	ack = ntohl(tcph->ack_seq);
	this_seq = sock.tcp_sk->this.seq;
	this_unack = sock.tcp_sk->this_unack;
	if ((this_unack < ack) && (ack <= this_seq)) {
		free_rexmitting_queue(sock, ack, this_unack);
		sock.tcp_sk->this_unack = ack;
	}
	else if (ack == this_unack) { } /* no new acknowledgments */
	else if (ack < this_unack) {
//		tcp_rexmit(sock);
	}
	else {
		assert(ack > this_seq);
		debug_print(1, "Error in process_ack: this_unack=%u ack=%u this_seq=%u\n", this_unack, ack, this_seq);
		switch (sock.sk->sk_state) {
		default:
			return TCP_RET_DROP;
		case TCP_LISTEN:
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
		case TCP_SYN_RECV:
			out_tcph->rst = 1;
			/* Set seq and ack */
			sock.tcp_sk->this_unack = ack;
			sock.tcp_sk->rem.seq = 0;
			return TCP_RET_SEND;
		}
	}

	/* Check ack flag for our state */
	switch (sock.sk->sk_state) {
	default:
		break;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_FINWAIT_1:
	case TCP_CLOSING:
	case TCP_LASTACK:
		if (ack >= sock.tcp_sk->ack_flag) { } /* All ok, our flag was confirmed */
		else { /* Else unmark ack flag */
			debug_print(1, "process_ack: sk 0x%p unmark ack\n", sock.tcp_sk);
			tcph->ack = 0;
		}
		break;
	}
	return TCP_RET_OK;
}

static int pre_process(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	int res;
	__u32 seq, seq_last, rem_seq, rem_last;

	switch (sock.sk->sk_state) {
	default:
		break;
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		seq = ntohl(tcph->seq);
		seq_last = seq + tcp_seq_len(*pskb) - 1;
		rem_seq = sock.tcp_sk->rem.seq;
		rem_last = rem_seq + sock.tcp_sk->this.wind;
		if ((rem_seq <= seq) && (seq < rem_last)) {
			if (rem_seq != seq) {
				/* TODO There is correct packet (with correct sequence
				 * number, but some packages was lost. We should save
				 * this skb, and wait previous packages.
				 */
				return TCP_RET_DROP;
			}
		}
		else if ((rem_seq <= seq_last) && (seq_last < rem_last)) { }
		else {
			debug_print(1, "Error in pre_process: rem_seq=%u seq=%u seq_last=%u rem_last=%u\n", rem_seq, seq, seq_last, rem_last);
			if ((seq < rem_seq) && (seq_last < rem_seq)) {
				/* Send segment with ack flag if this packet is duplicated */
				out_tcph->ack = 1;
				return TCP_RET_ACKN;
			}
			return TCP_RET_DROP;
		}
		break;
	}

	if (tcph->rst &&
			((res = process_rst(sock, tcph, out_tcph)) != TCP_RET_OK)) {
		return res;
	}

	if (tcph->ack &&
			((res = process_ack(sock, tcph, out_tcph)) != TCP_RET_OK)) {
		return res;
	}

	return TCP_RET_OK;
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
static const tcp_handler_t tcp_st_handler[TCP_MAX_STATE] = {
		[TCP_CLOSED]       = tcp_st_closed,
		[TCP_LISTEN]       = tcp_st_listen,
		[TCP_SYN_SENT]     = tcp_st_syn_sent,
		[TCP_SYN_RECV_PRE] = tcp_st_syn_recv_pre,
		[TCP_SYN_RECV]     = tcp_st_syn_recv,
		[TCP_ESTABIL]      = tcp_st_estabil,
		[TCP_FINWAIT_1]    = tcp_st_finwait_1,
		[TCP_FINWAIT_2]    = tcp_st_finwait_2,
		[TCP_CLOSEWAIT]    = tcp_st_closewait,
		[TCP_CLOSING]      = tcp_st_closing,
		[TCP_LASTACK]      = tcp_st_lastack,
		[TCP_TIMEWAIT]     = tcp_st_timewait
};

static int tcp_handle(union sock_pointer sock, struct sk_buff *skb, tcp_handler_t hnd) {
	/* If result is not TCP_RET_OK then further processing can't be made */
	int res;
	unsigned char out_tcph_raw[TCP_V4_HEADER_MIN_SIZE];

	assert(hnd != NULL);

	memset(out_tcph_raw, 0, sizeof out_tcph_raw);

	/* If we want to use the current package (to save for example),
	 * we must create a new package to send a response (if we need
	 * to send a response). After this the handler may do anything
	 * with current sk_buff_t.
	 */
	res = (*hnd)(sock, &skb, skb->h.th, (struct tcphdr *)out_tcph_raw);
	debug_print(2, "tcp_handle: ret %d skb 0x%p sk 0x%p\n", res, skb, sock.tcp_sk);
	switch (res) {
	default: /* error code, for instance */
	case TCP_RET_DROP:
		kfree_skb(skb);
		break;
	case TCP_RET_SEND:
	case TCP_RET_ACKN:
		assert(skb != NULL);
		memcpy(skb->h.raw, out_tcph_raw, sizeof out_tcph_raw);
		skb->len = ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE;
		skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE);
		skb->h.th->doff = TCP_V4_HEADER_MIN_SIZE / 4;
		if (res == TCP_RET_SEND) {
			send_from_sock(sock, skb);
		}
		else {
			send_ack_from_sock(sock, skb);
		}
		break;
	case TCP_RET_OK:
		break;
	}

	return res;
}

static struct tcp_sock * tcp_lookup(in_addr_t saddr, __be16 sport, in_addr_t daddr, __be16 dport) {
	size_t i;
	union sock_pointer sock;

	/* lookup socket with strict addressing */
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		if (((sock.tcp_sk = tcp_hash[i]) != NULL)
				&& ((sock.inet_sk->rcv_saddr == saddr) && (sock.inet_sk->sport == sport)
				&& (sock.inet_sk->daddr == daddr) && (sock.inet_sk->dport == dport))) {
			return sock.tcp_sk;
		}
	}

	/* lookup another sockets */
	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		if (((sock.tcp_sk = tcp_hash[i]) != NULL)
				&& (((sock.inet_sk->rcv_saddr == INADDR_ANY) || (sock.inet_sk->rcv_saddr == saddr))
				&& (sock.inet_sk->sport == sport))) {
			return sock.tcp_sk;
		}
	}

	return NULL;
}

/**
 * Main function of TCP protocol
 */
static void tcp_process(union sock_pointer sock, struct sk_buff *skb) {
	switch (tcp_handle(sock, skb, pre_process)) {
	default:
	case TCP_RET_DROP:
		tcp_rexmit(sock);
		break;
	case TCP_RET_OK:
		assert(sock.sk->sk_state < TCP_MAX_STATE);
		tcp_handle(sock, skb, tcp_st_handler[sock.sk->sk_state]);
		break;
	case TCP_RET_SEND:
	case TCP_RET_ACKN:
		break;
	}
}

static int tcp_v4_rcv(struct sk_buff *skb) {
	iphdr_t *iph;
	tcphdr_t *tcph;
	union sock_pointer sock;

	assert(skb != NULL);

	iph = ip_hdr(skb);
	tcph = tcp_hdr(skb);
	sock.tcp_sk = tcp_lookup(iph->daddr, tcph->dest, iph->saddr, tcph->source);
	packet_print(sock, skb, "=>", skb->nh.iph->saddr, skb->h.th->source);
	if (sock.tcp_sk == NULL) {
		tcp_handle(tcp_default, skb, tcp_st_handler[TCP_CLOSED]);
	}
	else {
		tcp_process(sock, skb);
	}

	return ENOERR;
}

/*static*/ void timer_handler(sys_timer_t* timer, void *param) {
	struct list_head *resocks = (struct list_head *) param;
	union sock_pointer sock;

	list_for_each_entry(sock.tcp_sk, resocks, rexmit_link) {
		tcp_rexmit(sock);
	}
}

static int tcp_v4_init(void) {
	int res;
//	sys_timer_t *timer;

//	return timer_set(&timer, REXMIT_DELAY, timer_handler, (void *)&rexmit_socks);
	/* Create a maintainance tcp socket without inheritors */
	tcp_default.sk = inet_create_sock(0, &tcp_prot, IPPROTO_TCP, SOCK_STREAM);
	if (tcp_default.sk == NULL) {
		return -ENOMEM;
	}
	res = tcp_prot.init(tcp_default.sk);
	if (res < 0) {
		sk_common_release(tcp_default.sk);
		return res;
	}
	tcp_prot.unhash(tcp_default.sk);
	return ENOERR;
}


/************************ Socket's functions ***************************/
static int tcp_v4_init_sock(struct sock *sk) {
	union sock_pointer sock;
	struct sk_buff_head *queue;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_init_sock: sk 0x%p\n", sock.tcp_sk);

	queue = alloc_skb_queue();
	if (queue == NULL) {
		return -ENOMEM;
	}
	tcp_set_st(sock, TCP_CLOSED);
	sock.tcp_sk->this_unack = 100; // TODO remove constant
	sock.tcp_sk->seq_queue = sock.tcp_sk->this.seq = sock.tcp_sk->this_unack;
	sock.tcp_sk->this.wind = TCP_WINDOW_DEFAULT;
	sock.tcp_sk->lock = 0;
	sock.tcp_sk->conn_wait = queue;
	list_add(&sock.tcp_sk->rexmit_link, &rexmit_socks);

	return ENOERR;
}

static int tcp_v4_connect(struct sock *sk, struct sockaddr *addr, int addr_len) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;
	struct sockaddr_in *addr_in;
	struct rt_entry *rte;

	assert(sk != NULL);
	assert(addr != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_connect: sk 0x%p\n", sock.tcp_sk);

	switch (sock.sk->sk_state) {
	default:
		return -EBADF;
	case TCP_CLOSED:
	case TCP_SYN_RECV_PRE:
		if (addr_len != sizeof(struct sockaddr_in)) {
			return -EINVAL;
		}
		addr_in = (struct sockaddr_in *)addr;
		/* setup inet_sock */
		rte = rt_fib_get_best(addr_in->sin_addr.s_addr);
		if (rte == NULL) {
			return -EHOSTUNREACH;
		}
		assert(in_dev_get(rte->dev) != NULL);
		sock.inet_sk->saddr = in_dev_get(rte->dev)->ifa_address; // TODO remove this
		sock.inet_sk->daddr = addr_in->sin_addr.s_addr;
		sock.inet_sk->dport = addr_in->sin_port;
		/* make skb */
		skb = alloc_prep_skb(0);
		if (skb == NULL) {
			return -ENOMEM;
		}
		tcph = tcp_hdr(skb);
		tcph->syn = 1;
		sock.tcp_sk->seq_queue += tcp_seq_len(skb);
		tcp_set_st(sock, TCP_SYN_SENT);
		send_from_sock(sock, skb);
		while ((sk->sk_state != TCP_ESTABIL) && (sk->sk_state != TCP_CLOSED));
		return (sk->sk_state == TCP_ESTABIL ? ENOERR : -1);
	case TCP_LISTEN:
		return -1;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -1; /* error: connection already exists */
	}
}

static int tcp_v4_listen(struct sock *sk, int backlog) {
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_listen: sk 0x%p\n", sock.tcp_sk);

	switch (sock.sk->sk_state) {
	default:
		return -EBADF;
	case TCP_CLOSED:
	case TCP_LISTEN:
		tcp_set_st(sock, TCP_LISTEN);
		return ENOERR;
	case TCP_SYN_RECV_PRE:
		return -1; /* TODO */
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -1; /* error: connection already exists */
	}
}

static int tcp_v4_accept(struct sock *sk, struct sock *newsk,
		struct sockaddr *addr, int *addr_len) {
	struct sk_buff *skb;
	union sock_pointer sock, newsock;
	struct sockaddr_in *addr_in;

	assert(sk != NULL);
	assert(newsk != NULL);
	assert(addr != NULL);
	assert(addr_len != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_accept: sk 0x%p\n", sock.tcp_sk);

	switch (sock.sk->sk_state) {
	default: /* TODO another states */
		return -EBADF;
	case TCP_LISTEN:
		newsock.sk = newsk;
		addr_in = (struct sockaddr_in *)addr;
		do {
			/* waiting for clients */
			while ((skb = skb_dequeue(sock.tcp_sk->conn_wait)) == NULL);
			/* save remote address */
			addr_in->sin_family = AF_INET;
			addr_in->sin_port = skb->h.th->source;
			addr_in->sin_addr.s_addr = skb->nh.iph->saddr;
			debug_print(4, "tcp_v4_accept: new sk 0x%x for %s:%d\n", (int)newsk,
					inet_ntoa(*(struct in_addr *)&skb->nh.iph->saddr), (int)ntohs(skb->h.th->source));
			/* set up new socket */
			newsock.inet_sk->saddr = newsock.inet_sk->rcv_saddr = skb->nh.iph->daddr;
			newsock.inet_sk->sport = skb->h.th->dest;
			newsock.inet_sk->daddr = skb->nh.iph->saddr;
			newsock.inet_sk->dport = skb->h.th->source;
			/* processing of skb */
			tcp_set_st(newsock, TCP_SYN_RECV_PRE);
			tcp_process(newsock, skb);
			/* wait until something happened */
			while ((newsock.sk->sk_state != TCP_ESTABIL) && (newsock.sk->sk_state != TCP_CLOSED));
		} while (newsock.sk->sk_state != TCP_ESTABIL); /* repeat until new connection is not established */
		*addr_len = sizeof(struct sockaddr_in);
		return ENOERR;
	}
}

static int tcp_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len) {
	struct sk_buff *skb;
	union sock_pointer sock;

	assert(sk != NULL);
	assert(msg != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_sendmsg: sk 0x%p\n", sock.tcp_sk);

	switch (sock.sk->sk_state) {
	default:
		return -EBADF;
	case TCP_CLOSED:
	case TCP_LISTEN:
	case TCP_SYN_RECV_PRE:
		return -1; /* error: connection does not exist */
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
		return -1; /* TODO save data and send them later */
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
		skb = alloc_prep_skb(msg->msg_iov->iov_len);
		if (skb == NULL) {
			return -ENOMEM;
		}

		memcpy((void*)(skb->h.raw + TCP_V4_HEADER_MIN_SIZE),
					(void *)msg->msg_iov->iov_base, msg->msg_iov->iov_len);

		/* Fill TCP header */
		skb->h.th->psh = 1; /* XXX not req */
		skb->h.th->ack = 1;
		debug_print(4, "tcp_v4_sendmsg: sending len %d, unack %u, seq %u\n", len,
				sock.tcp_sk->this_unack, sock.tcp_sk->this.seq);
		sock.tcp_sk->seq_queue += tcp_seq_len(skb);
		return send_from_sock(sock, skb);
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -1; /* error: connection closing */
	}
}

static int tcp_v4_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
			size_t len, int noblock, int flags) {
	struct sk_buff *skb;
	union sock_pointer sock;
	size_t data_left;

	assert(sk != NULL);
	assert(msg != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_recvmsg: sk 0x%p\n", sock.tcp_sk);

check_state:
	switch (sock.sk->sk_state) {
	default:
		return -EBADF;
	case TCP_CLOSED:
	case TCP_LISTEN:
		return -1; /* error: connection does not exist */
	case TCP_SYN_SENT:
	case TCP_SYN_RECV_PRE:
	case TCP_SYN_RECV:
		return -1; /* TODO this in tcp_st_xxx function*/
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
		skb = skb_peek_datagram(sk, flags, 0, 0);
		if (skb == NULL) {
			if (sock.sk->sk_state == TCP_CLOSEWAIT) {
				return -1; /* error: connection closing */
			}
			/* wait received packet or another state */
			goto check_state;
		}
		data_left = tcp_data_left(skb);
		len = (len >= data_left ? data_left : len);
		msg->msg_iov->iov_len = len;
		memcpy((void *)msg->msg_iov->iov_base, skb->p_data, len);
		skb->p_data += len;
		/* remove skb from receive's queue if he doesn't contains more data */
		if (len >= data_left) {
				kfree_skb(skb);
		}
		return ENOERR;
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return -1; /* error: connection closing */
	}
}

static void tcp_v4_close(struct sock *sk, long timeout) {
	struct sk_buff *skb;
	struct tcphdr *tcph;
	union sock_pointer sock;

	assert(sk != NULL);

	sock.sk = sk;
	debug_print(4, "tcp_v4_close: sk 0x%p\n", sock.tcp_sk);

	switch (sock.sk->sk_state) {
	default:
		return; /* error: EBADF */
	case TCP_CLOSED:
		tcp_free_sock(sock);
		return; /* error: connection does not exist */
	case TCP_LISTEN:
	case TCP_SYN_SENT:
	case TCP_SYN_RECV_PRE:
		tcp_set_st(sock, TCP_CLOSED);
		tcp_free_sock(sock);
		return;
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
		skb = alloc_prep_skb(0);
		if (skb == NULL) {
			return;
		}
		tcph = tcp_hdr(skb);
		tcph->fin = 1;
		tcph->ack = 1;
		sock.tcp_sk->seq_queue += tcp_seq_len(skb);
		tcp_set_st(sock, (sock.sk->sk_state == TCP_CLOSEWAIT ? TCP_LASTACK : TCP_FINWAIT_1));
		send_from_sock(sock, skb);
		break;
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
		return; /* error: connection closing */
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return; /* error: connection closing */
	}
	while (sock.sk->sk_state != TCP_CLOSED);
}

//TODO move to hash table routines
static void tcp_v4_hash(struct sock *sk) {
	size_t i;

	debug_print(4, "tcp_v4_hash: sk 0x%p\n", sk);
	for (i = 0; i< CONFIG_MAX_KERNEL_SOCKETS; ++i) {
		if (tcp_hash[i] == NULL) {
			tcp_hash[i] = (struct tcp_sock *)sk;
			break;
		}
	}
}

static void tcp_v4_unhash(struct sock *sk) {
	size_t i;

	debug_print(4, "tcp_v4_unhash: sk 0x%p\n", sk);
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
		.init       = tcp_v4_init_sock,
		.connect    = tcp_v4_connect,
		.listen     = tcp_v4_listen,
		.accept     = tcp_v4_accept,
//		.setsockopt = tcp_v4_setsockopt,
//		.getsockopt = tcp_v4_getsockopt,
		.sendmsg    = tcp_v4_sendmsg,
		.recvmsg    = tcp_v4_recvmsg,
		.close      = tcp_v4_close,
		.hash       = tcp_v4_hash,
		.unhash     = tcp_v4_unhash,
		.sock_alloc = tcp_v4_sock_alloc,
		.sock_free  = tcp_v4_sock_free,
		.obj_size   = sizeof(struct tcp_sock),
};
