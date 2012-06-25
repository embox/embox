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
#include <kernel/prom_printf.h>

#include <net/inetdevice.h>
#include <net/socket.h>
#include <net/checksum.h>
#include <net/route.h>
#include <net/skbuff.h>
#include <errno.h>
#include <err.h>
#include <assert.h>
#include <net/sock.h>
#include <time.h>

#include <kernel/timer.h>
#include <embox/net/proto.h>
#include <kernel/softirq_lock.h>
#include <kernel/time/time_types.h>
#include <kernel/time/ktime.h>


EMBOX_NET_PROTO_INIT(IPPROTO_TCP, tcp_v4_rcv, NULL, tcp_v4_init);


/** TODO
 * +1. Create default socket for resetting
 * +2. PSH flag
 * +3. RST flag
 * ?4. Changes state's logic (i.e. TCP_CLOSED for all socket which doesn't exists etc.)
 * +5. Rewrite send_from_sock (don't send new skb if queue is not empty)
 * +6. tcp_sock_free in tcp_st_finwait_2
 * 7. Remove seq_queue (use rem.seq instead, build packet, and then rebuild only)
 * 8. Add lock/unlock
 * +9. Add rexmit
 * +-10. Add window
 * 11. Add options
 * +-12. Add timeout (i.e. sockopts) to tcp_sock.c
 */


/* Error code of TCP handlers */
enum {
	TCP_RET_OK,     /* all ok, don't free packet */
	TCP_RET_DROP,   /* drop packet */
	TCP_RET_SEND,   /* send answer */
	TCP_RET_ACK,    /* send acknowledgment */
	TCP_RET_RST,    /* reset (Only for pre_process) */
	TCP_RET_FREE    /* drop packet and free socket */
};

/* Type of TCP state handlers */
typedef int (*tcp_handler_t)(union sock_pointer sock,
		struct sk_buff **skb, struct tcphdr *tcph, struct tcphdr *out_tcph);


struct tcp_sock *tcp_table[CONFIG_MAX_KERNEL_SOCKETS]; /* All TCP sockets in system */
static union sock_pointer tcp_sock_default; /* Default socket for TCP protocol. */
static struct sys_timer tcp_tmr_default; /* Timer structure for rexmitting or TIME-WAIT satate */

/* Prototypes */
extern struct sock * inet_create_sock(gfp_t priority, struct proto *prot, int protocol, int type);
static int tcp_handle(union sock_pointer sock, struct sk_buff *skb, tcp_handler_t hnd);
static const tcp_handler_t tcp_st_handler[];


/************************ Debug functions ******************************/

void debug_print(__u8 code, const char *msg, ...) {
	va_list args;
	return;				/* YOUR output is too annoying for others */

	va_start(args, msg);
	switch (code) {
//default:
	case 0:  /* default */
	case 1:  /* in/out package print */
//	case 2:  /* socket state */
	case 3:  /* global functions */
//	case 4:  /* hash/unhash */
//	case 5:  /* lock/unlock */
	case 6:	 /* sock_alloc/sock_free */
//	case 7:  /* tcp_default_timer action */
//	case 8:  /* state's handler */
//	case 9:  /* sending package */
//	case 10: /* pre_process */
//	case 11: /* tcp_handle */
		softirq_lock();
		prom_vprintf(msg, args);
		softirq_unlock();
		break;
	}
	va_end(args);
}

static inline void packet_print(union sock_pointer sock, struct sk_buff *skb, char *msg,
		in_addr_t ip, uint16_t port) {
	debug_print(1, "%lu %s:%d %s sk 0x%p skb 0x%p seq %u ack %u seq_len %u flags %s %s %s %s %s %s %s %s\n",
			// info
			clock(), inet_ntoa(*(struct in_addr*)&ip), ntohs(port), msg, sock.tcp_sk, skb,
			// seq, ack, seq_len
			ntohl(skb->h.th->seq), ntohl(skb->h.th->ack_seq), tcp_seq_len(skb),
			// flags
			(skb->h.th->ack ? "ACK" : ""), (skb->h.th->syn ? "SYN" : ""),
			(skb->h.th->fin ? "FIN" : ""), (skb->h.th->rst ? "RST" : ""),
			(skb->h.th->psh ? "PSH" : ""), (skb->h.th->urg ? "URG" : ""),
			(skb->h.th->ece ? "ECE" : ""), (skb->h.th->cwr ? "CWR" : ""));
}


/************************ Auxiliary functions **************************/
struct sk_buff * alloc_prep_skb(size_t addit_len) {
	struct sk_buff *skb;

	skb = alloc_skb(ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE +
				    /*inet->opt->optlen +*/ TCP_V4_HEADER_MIN_SIZE +
				    addit_len, 0);
	if (skb == NULL) {
		LOG_ERROR("no memory or len is too big. len=%u\n", addit_len);
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

void tcp_obj_lock(union sock_pointer sock, int obj) {
	__u8 locked;

	size_t tmp_bug_fix = 0;
	/* FIXME `tmp_bug_fix` is temporary variable, which help fix the next bug:
	 * For example, we locked TCP_SYNC_STATE by tcp_obj_lock(sock, TCP_SYNC_STATE).
	 * It set some bit of `lock` variable. Then if any packages was received
	 * there handlers may want to lock the same objects. But we have some problem..
	 * When we processing received packages, current thread has a higher priority
	 * than user's thread, so we has infinite loop waiting for unlock this bit..
	 */

	debug_print(5, "tcp_obj_lock: try sk 0x%p (%x/%x)\n", sock.tcp_sk, obj, sock.tcp_sk->lock);
try_lock:
	softirq_lock();
	locked = sock.tcp_sk->lock & obj, sock.tcp_sk->lock |= obj;
	softirq_unlock();
	if (++tmp_bug_fix > 1000) goto lock_done;
	if (!locked) {
		goto lock_done;
	}
	goto try_lock;
lock_done:
	debug_print(5, "tcp_obj_lock: sk 0x%p (%x/%x) locked\n", sock.tcp_sk, obj, sock.tcp_sk->lock);
}

void tcp_obj_unlock(union sock_pointer sock, int obj) {
	debug_print(5, "tcp_obj_unlock: sk 0x%p (%x/%x) unlocked\n", sock.tcp_sk, obj, sock.tcp_sk->lock);
	softirq_lock();
	sock.tcp_sk->lock &= ~obj;
	softirq_unlock();
}

static size_t tcp_data_len(struct sk_buff *skb) {
	return ntohs(skb->nh.iph->tot_len) - IP_HEADER_SIZE(skb->nh.iph) - TCP_V4_HEADER_SIZE(skb->h.th);
}

size_t tcp_data_left(struct sk_buff *skb) {
	size_t size, ip_len;

	ip_len = ntohs(skb->nh.iph->tot_len);
	size = skb->p_data - skb->nh.raw;
	return (ip_len > size ? ip_len - size : 0);
}

static int tcp_seq_flags(struct tcphdr *tcph) {
	return (tcph->fin || tcph->syn);
}

size_t tcp_seq_len(struct sk_buff *skb) {
	size_t data_len;

	data_len = tcp_data_len(skb);
	return (data_len > 0 ? data_len : (size_t)tcp_seq_flags(skb->h.th));
}

static size_t tcp_seq_left(struct sk_buff *skb) {
	size_t data_left;

	data_left = tcp_data_left(skb);
	return (data_left > 0 ? data_left : (size_t)tcp_seq_flags(skb->h.th));
}

void tcp_set_st(union sock_pointer sock, unsigned char new_state) {
	const char *str_state[TCP_MAX_STATE] = {"TCP_CLOSED", "TCP_LISTEN",
			"TCP_SYN_SENT", "TCP_SYN_RECV_PRE", "TCP_SYN_RECV", "TCP_ESTABIL",
			"TCP_FINWAIT_1", "TCP_FINWAIT_2", "TCP_CLOSEWAIT", "TCP_CLOSING",
			"TCP_LASTACK", "TCP_TIMEWAIT"};

	switch (new_state) {
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_FINWAIT_1:
	case TCP_CLOSING:
	case TCP_LASTACK:
		sock.tcp_sk->ack_flag = sock.tcp_sk->seq_queue;
		debug_print(2, "sk 0x%p set ack_flag %u for state %d-%s\n",
				sock.tcp_sk, sock.tcp_sk->ack_flag, new_state, str_state[new_state]);
		break;
	}

	sock.sk->sk_state = new_state;
	debug_print(2, "sk 0x%p set state %d-%s\n", sock.tcp_sk, new_state, str_state[new_state]);
}

int tcp_st_status(union sock_pointer sock) {
	switch (sock.sk->sk_state) {
	default:
	case TCP_CLOSED:
	case TCP_LISTEN:
		return TCP_ST_NOTEXIST;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV_PRE:
	case TCP_SYN_RECV:
		return TCP_ST_NONSYNC;
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
		return TCP_ST_SYNC;
	}
}

useconds_t tcp_get_usec(void) {
	struct ktimeval tv;
	ktime_get_timeval(&tv);
	return (useconds_t)tv.tv_sec * USEC_PER_SEC + (useconds_t)tv.tv_usec;
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
		__be16 source, __be16 dest, __u32 seq, __u32 ack_seq,
		__u16 window, struct sk_buff *skb) {
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
	int res;
	skb->sk = sock.sk; // check it
	sock.tcp_sk->this.seq = sock.tcp_sk->this_unack + tcp_seq_len(skb);
	rebuild_tcp_header(sock.inet_sk->saddr, sock.inet_sk->daddr,
			sock.inet_sk->sport, sock.inet_sk->dport,
			sock.tcp_sk->this_unack, sock.tcp_sk->rem.seq,
			sock.tcp_sk->this.wind, skb);
	packet_print(sock, skb, "<=", sock.inet_sk->daddr, sock.inet_sk->dport);
	res = ip_send_packet(sock.inet_sk, skb);
	if (res < 0) {
		LOG_ERROR("ip_send_packet returned %d\n", res);
	}
	return res;
}

static int tcp_sock_xmit(union sock_pointer sock, int xmit_mod) {
	struct sk_buff *skb, *skb_send;
	size_t seq_len;

	/* check time wait */
	if (!(xmit_mod & TCP_XMIT_IGNORE_DELAY)
			&& (clock() - sock.tcp_sk->last_activity < TCP_REXMIT_DELAY)) {
		return ENOERR;
	}

	tcp_obj_lock(sock, TCP_SYNC_WRITE_QUEUE);
	/* get next skb for sending */
	skb = skb_peek(sock.sk->sk_write_queue);
	if (skb == NULL) {
		/* TODO
		 * seq_queue is set in the function up the stack,
		 * so assert may trigger falsely. Need add lock
		 * and after that it will be correct.
		 */
//		assert(sock.tcp_sk->this_unack == sock.tcp_sk->this.seq);
//		assert(sock.tcp_sk->this.seq == sock.tcp_sk->seq_queue);
		tcp_obj_unlock(sock, TCP_SYNC_WRITE_QUEUE);
		return ENOERR;
	}
	seq_len = tcp_seq_len(skb);
	if (seq_len > 0) {
		skb_send = skb_clone(skb, 0);
		if (skb_send == NULL) {
			LOG_ERROR("no memory\n");
			tcp_obj_unlock(sock, TCP_SYNC_WRITE_QUEUE);
			return -ENOMEM;
		}
		debug_print(9, "tcp_sock_xmit: send skb 0x%p, postponed 0x%p\n", skb_send, skb);
	}
	else {
		skb_send = skb_dequeue(sock.sk->sk_write_queue);
		assert(skb_send == skb);
		debug_print(9, "tcp_sock_xmit: send skb 0x%p\n", skb_send);
	}

	tcp_obj_unlock(sock, TCP_SYNC_WRITE_QUEUE);

	sock.tcp_sk->last_activity = tcp_get_usec(); /* set last xmit time */

	return tcp_xmit(sock, skb_send);
}

/**
 * Send a acknowledgment, only
 */
static void send_ack_from_sock(union sock_pointer sock, struct sk_buff *skb_ackn) {
	struct sk_buff *skb;

	/* FIXME skb may be freed by free_rexmitting_queue */
	skb = skb_peek(sock.sk->sk_write_queue);
	if (skb == NULL) {
		/* send skb_ackn if no skb in outgoing queue */
		debug_print(9, "send_ack_from_sock: send 0x%p\n", skb_ackn);
		tcp_xmit(sock, skb_ackn);
	}
	else {
		/* If there, set ack flag and free skb_ackn */
		skb->h.th->ack = 1;
		debug_print(9, "send_ack_from_sock: add ack to 0x%p\n", skb);
		kfree_skb(skb_ackn);

		tcp_sock_xmit(sock, TCP_XMIT_IGNORE_DELAY);
	}
}

/**
 * Send package
 */
void send_from_sock(union sock_pointer sock, struct sk_buff *skb_send, int xmit_mod) {
	/* correct xmit_mod if queue is empty */
	if (skb_peek(sock.sk->sk_write_queue) == NULL) { /* TODO it doesn't synchronize */
		xmit_mod |= TCP_XMIT_IGNORE_DELAY;
	}
	/* TODO
	 * used by free_rexmitting_queue but when called rexmit
	 * all data transfered, not from p_data pointer
	 */
	skb_send->p_data = skb_send->h.raw + TCP_V4_HEADER_SIZE(skb_send->h.th);
	/* save skb */
//	list_add_tail((struct list_head *)skb_send, (struct list_head *)sock.sk->sk_write_queue);
	skb_queue_tail(sock.sk->sk_write_queue, skb_send);
	debug_print(9, "send_from_sock: save 0x%p to outgoing queue\n", skb_send);
	/* send packet */
	tcp_sock_xmit(sock, xmit_mod);
}

static void free_rexmitting_queue(union sock_pointer sock, __u32 ack, __u32 unack) {
	struct sk_buff *sent_skb;
	size_t ack_len, seq_left;

	ack_len = ack - unack;
	while ((ack_len != 0)
			&& ((sent_skb = skb_peek(sock.sk->sk_write_queue)) != NULL)) {
		seq_left = tcp_seq_left(sent_skb);
		if (seq_left <= ack_len) {
			ack_len -= seq_left;
			debug_print(9, "free_rexmitting_queue: remove skb 0x%p\n", sent_skb);
			kfree_skb(sent_skb); /* list_del will done at kfree_skb */
		}
		else {
			sent_skb->p_data += ack_len;
			break;
		}
	}
}

void tcp_free_sock(union sock_pointer sock) {
	union sock_pointer anticipant;

	list_for_each_entry(anticipant.tcp_sk, &sock.tcp_sk->conn_wait, conn_wait) {
		sk_common_release(anticipant.sk); /* TODO send RST before */
	}

	sk_common_release(sock.sk);
#if 0 /* TODO.. this is a good test for checking sync */
	for (i = 0; i < sizeof tcp_table / sizeof tcp_table[0]; ++i) {
		sk_common_release((struct sock *)tcp_hash[i]);
	}
#endif
}


/************************ Handlers of TCP states ***********************/
static int tcp_st_closed(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_closed\n");
	assert(sock.sk->sk_state == TCP_CLOSED);

	out_tcph->rst = 1;
	/* Set seq and ack */
	if (tcph->ack) {
		sock.tcp_sk->this_unack = ntohl(tcph->ack_seq);
		sock.tcp_sk->rem.seq = 0;
	}
	else {
		out_tcph->ack = 1;
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
	union sock_pointer newsock;

	debug_print(8, "call tcp_st_listen\n");
	assert(sock.sk->sk_state == TCP_LISTEN);

	if (tcph->syn) {
		/* Allocate new socket for this connection */
		newsock.sk = inet_create_sock(0, (struct proto *)&tcp_prot, SOCK_STREAM, IPPROTO_TCP);
		if (newsock.sk == NULL) {
			LOG_ERROR("no memory\n");
			return -ENOMEM;
		}
		debug_print(8, "\t append sk 0x%p for skb 0x%p to sk 0x%p queue\n", newsock.tcp_sk, *pskb, sock.tcp_sk);
		/* Set up new socket */
		newsock.inet_sk->saddr = newsock.inet_sk->rcv_saddr = (*pskb)->nh.iph->daddr;
		newsock.inet_sk->sport = (*pskb)->h.th->dest;
		newsock.inet_sk->daddr = (*pskb)->nh.iph->saddr;
		newsock.inet_sk->dport = (*pskb)->h.th->source;
		/* Handling skb */
		tcp_set_st(newsock, TCP_SYN_RECV_PRE); /* TODO remove this */
		tcp_handle(newsock, *pskb, tcp_st_handler[TCP_SYN_RECV_PRE]);
		tcp_obj_lock(sock, TCP_SYNC_CONN_QUEUE);
		list_add_tail(&newsock.tcp_sk->conn_wait, &sock.tcp_sk->conn_wait);
		tcp_obj_unlock(sock, TCP_SYNC_CONN_QUEUE);
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
		sock.tcp_sk->rem.wind = ntohs(tcph->window);
		out_tcph->ack = 1;
		if (tcph->ack) {
			tcp_set_st(sock, TCP_ESTABIL);
		}
		else {
			tcp_set_st(sock, TCP_SYN_RECV);
		}
		return TCP_RET_ACK;
	}

	return TCP_RET_DROP;
}

static int tcp_st_syn_recv_pre(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_syn_recv_pre\n");
	assert(sock.sk->sk_state == TCP_SYN_RECV_PRE);

	if (tcph->syn) {
		sock.tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
		sock.tcp_sk->rem.wind = ntohs(tcph->window);
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
		return TCP_RET_ACK;
	}
	else if (tcph->fin) {
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack = 1;
		tcp_set_st(sock, TCP_CLOSEWAIT);
		return TCP_RET_ACK;
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
				tcp_set_st(sock, TCP_TIMEWAIT);
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
		return TCP_RET_ACK;
	}
	else if (tcph->fin) {
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack = 1;
		if (tcph->ack) {
			tcp_set_st(sock, TCP_TIMEWAIT);
		}
		else {
			tcp_set_st(sock, TCP_CLOSING);
		}
		return TCP_RET_ACK;
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
			tcp_set_st(sock, TCP_TIMEWAIT);
		}
		*pskb = answer;
		return TCP_RET_ACK;
	}
	else if (tcph->fin) {
		sock.tcp_sk->rem.seq += 1;
		out_tcph->ack = 1;
		tcp_set_st(sock, TCP_TIMEWAIT);
		return TCP_RET_ACK;
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
		tcp_set_st(sock, TCP_TIMEWAIT);
	}

	return TCP_RET_DROP;
}

static int tcp_st_lastack(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	debug_print(8, "call tcp_st_lastack\n");
	assert(sock.sk->sk_state == TCP_LASTACK);

	if (tcph->ack) {
		tcp_set_st(sock, TCP_CLOSED);
		return TCP_RET_FREE;
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
	tcp_obj_lock(sock, TCP_SYNC_STATE);
	switch (sock.sk->sk_state) {
	default:
		if ((sock.sk->sk_state == TCP_SYN_SENT)
				&& (sock.tcp_sk->this.seq != ntohl(tcph->ack_seq))) { // TODO take this.seq from array
			break; /* invalid reset */
		}
		tcp_set_st(sock, TCP_CLOSED);
		break;
	case TCP_CLOSED:
	case TCP_LISTEN:
	case TCP_SYN_RECV_PRE:
		break;
	}
	tcp_obj_unlock(sock, TCP_SYNC_STATE);
	return TCP_RET_DROP;
}

static int process_ack(union sock_pointer sock, struct tcphdr *tcph,
		struct tcphdr *out_tcph) {
	__u32 ack, this_seq, this_unack;

	/* Resetting if recv ack in this state */
	switch (sock.sk->sk_state) {
	default:
		break;
	case TCP_CLOSED:
	case TCP_LISTEN:
		return TCP_RET_RST;
	}

	ack = ntohl(tcph->ack_seq);
	this_seq = sock.tcp_sk->this.seq;
	this_unack = sock.tcp_sk->this_unack;
	if ((this_unack < ack) && (ack <= this_seq)) {
		tcp_obj_lock(sock, TCP_SYNC_WRITE_QUEUE);
		free_rexmitting_queue(sock, ack, this_unack);
		sock.tcp_sk->this_unack = ack;
		tcp_obj_unlock(sock, TCP_SYNC_WRITE_QUEUE);
		tcp_sock_xmit(sock, TCP_XMIT_IGNORE_DELAY); /* rexmit next data from queue */
	}
	else if (ack == this_unack) { } /* no new acknowledgments */
	else if (ack < this_unack) { } /* package with non-last acknowledgment */
	else {
		assert(ack > this_seq);
		debug_print(10, "process_ack: invalid acknowledgments: this_unack=%u ack=%u this_seq=%u\n", this_unack, ack, this_seq);
		switch (sock.sk->sk_state) {
		default:
			return TCP_RET_DROP;
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
		case TCP_SYN_RECV:
			return TCP_RET_RST;
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
			debug_print(10, "process_ack: sk 0x%p unmark ack\n", sock.tcp_sk);
			tcph->ack = 0;
		}
		break;
	}
	return TCP_RET_OK;
}

static int pre_process(union sock_pointer sock, struct sk_buff **pskb,
		struct tcphdr *tcph, struct tcphdr *out_tcph) {
	int res;
	__u16 check;
	__u32 seq, seq_last, rem_seq, rem_last;

	/* Check CRC */
	check = tcph->check;
	tcph->check = 0;
	if (check != tcp_checksum(sock.inet_sk->daddr, sock.inet_sk->saddr,
			IPPROTO_TCP, tcph, TCP_V4_HEADER_SIZE(tcph) + tcp_data_len(*pskb))) {
		LOG_ERROR("invalid ckecksum %x sk 0x%p skb 0x%p\n", (int)check,
						sock.tcp_sk, *pskb);
//		return TCP_RET_DROP;
	}

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
			debug_print(10, "pre_process: received old package: rem_seq=%u seq=%u seq_last=%u rem_last=%u\n", rem_seq, seq, seq_last, rem_last);
			if ((seq < rem_seq) && (seq_last < rem_seq)) {
				/* Send segment with ack flag if this packet is duplicated */
				out_tcph->ack = 1;
				return TCP_RET_ACK;
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

	memset(out_tcph_raw, 0, sizeof out_tcph_raw);

	/* If we want to use the current package (to save for example),
	 * we must create a new package to send a response (if we need
	 * to send a response). After this the handler may do anything
	 * with current sk_buff_t.
	 * If hnd is NULL we use synchronization tools.
	 */
	if (hnd == NULL ) {
		tcp_obj_lock(sock, TCP_SYNC_STATE);
		assert(sock.sk->sk_state < TCP_MAX_STATE);
		assert(tcp_st_handler[sock.sk->sk_state] != NULL);
		res = tcp_st_handler[sock.sk->sk_state](sock, &skb, skb->h.th, (struct tcphdr *)out_tcph_raw);
		tcp_obj_unlock(sock, TCP_SYNC_STATE);
	}
	else {
		res = hnd(sock, &skb, skb->h.th, (struct tcphdr *)out_tcph_raw);
	}
	debug_print(11, "tcp_handle: ret %d skb 0x%p sk 0x%p\n", res, skb, sock.tcp_sk);

	switch (res) {
	default: /* error code, TCP_RET_DROP, TCP_RET_FREE */
		kfree_skb(skb);
		break;
	case TCP_RET_SEND:
	case TCP_RET_ACK:
		assert(skb != NULL);
		memcpy(skb->h.raw, out_tcph_raw, sizeof out_tcph_raw);
		skb->len = ETH_HEADER_SIZE + IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE;
		skb->nh.iph->tot_len = htons(IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE);
		skb->h.th->doff = TCP_V4_HEADER_MIN_SIZE / 4;
		if (res == TCP_RET_SEND) {
			send_from_sock(sock, skb, TCP_XMIT_DEFAULT);
		}
		else {
			send_ack_from_sock(sock, skb);
		}
		break;
	case TCP_RET_RST: /* this processing in tcp_process */
	case TCP_RET_OK:
		break;
	}

	return res;
}

static struct tcp_sock * tcp_lookup(in_addr_t saddr, __be16 sport, in_addr_t daddr, __be16 dport) {
	size_t i;
	union sock_pointer sock;

	/* lookup socket with strict addressing */
	for (i = 0; i < sizeof tcp_table / sizeof tcp_table[0]; ++i) {
		if (((sock.tcp_sk = tcp_table[i]) != NULL)
				&& ((sock.inet_sk->rcv_saddr == saddr) && (sock.inet_sk->sport == sport)
				&& (sock.inet_sk->daddr == daddr) && (sock.inet_sk->dport == dport))) {
			return sock.tcp_sk;
		}
	}

	/* lookup another sockets */
	for (i = 0; i < sizeof tcp_table / sizeof tcp_table[0]; ++i) {
		if (((sock.tcp_sk = tcp_table[i]) != NULL)
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
	sock.tcp_sk->last_activity = tcp_get_usec(); /* set last activity time */

	switch (tcp_handle(sock, skb, pre_process)) {
	default: /* error code and other TCP_RET_XXX */
		break;
	case TCP_RET_OK:
		switch (tcp_handle(sock, skb, NULL)) {
		default:
			break;
		case TCP_RET_FREE:
			tcp_free_sock(sock);
			break;
		}
		break;
	case TCP_RET_RST:
		tcp_handle(tcp_sock_default, skb, tcp_st_handler[TCP_CLOSED]);
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
		tcp_handle(tcp_sock_default, skb, tcp_st_handler[TCP_CLOSED]);
	}
	else {
		tcp_process(sock, skb);
	}

	return ENOERR;
}

static void tcp_tmr_timewait(union sock_pointer sock) {
	assert(sock.sk->sk_state == TCP_TIMEWAIT);
	if (clock() - sock.tcp_sk->last_activity >= TCP_TIMEWAIT_DELAY) {
		tcp_set_st(sock, TCP_CLOSED);
		debug_print(7, "TIMER: tcp_tmr_timewait: release sk 0x%p\n", sock.tcp_sk);
		sk_common_release(sock.sk);
	}
}

static void tcp_tmr_rexmit(union sock_pointer sock) {
	tcp_sock_xmit(sock, TCP_XMIT_DEFAULT);
}

static void tcp_timer_handler(struct sys_timer *timer, void *param) {
	size_t i;
	union sock_pointer sock;

//	debug_print(7, "TIMER: call tcp_timer_handler\n");

	for (i = 0; i < sizeof tcp_table / sizeof tcp_table[0]; ++i) {
		sock.tcp_sk = tcp_table[i];
		if (sock.tcp_sk == NULL) {
			continue;
		}
		if (sock.sk->sk_state == TCP_TIMEWAIT) {
			tcp_tmr_timewait(sock);
		}
		else if (tcp_st_status(sock) != TCP_ST_NOTEXIST) {
			tcp_tmr_rexmit(sock);
		}
	}
}

static int tcp_v4_init(void) {
	int res;

	/* Init global variables */
	memset(tcp_table, 0, sizeof tcp_table);

	/* Create default timer */
	res = timer_init(&tcp_tmr_default, TIMER_PERIODIC, TCP_TIMER_FREQUENCY, tcp_timer_handler, NULL);
	if (res < 0) {
		return res;
	}

	/* Create default socket */
	tcp_sock_default.sk = inet_create_sock(0, (struct proto *)&tcp_prot, IPPROTO_TCP, SOCK_STREAM);
	if (tcp_sock_default.sk == NULL) {
		return -ENOMEM;
	}
	tcp_prot.unhash(tcp_sock_default.sk);

	return ENOERR;
}

void * get_tcp_sockets(void) {
	return (void *)tcp_table;
}
