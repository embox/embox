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
#include <kernel/printk.h>
#include <stdio.h>
#include <util/log.h>

#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <poll.h>
#include <arpa/inet.h>

#include <net/l4/tcp.h>
#include <net/skbuff.h>
#include <net/sock.h>

#include <net/sock_wait.h>
#include <net/socket/inet_sock.h>
#include <net/socket/inet6_sock.h>
#include <net/l3/ipv4/ip.h>
#include <net/l3/ipv6.h>
#include <net/l2/ethernet.h>

#include <net/lib/ipv4.h>
#include <net/lib/ipv6.h>
#include <net/lib/tcp.h>

#include <kernel/time/timer.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/time/ktime.h>

#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_event.h>

#include <embox/net/pack.h>
#include <embox/net/proto.h>

EMBOX_NET_PROTO(ETH_P_IP, IPPROTO_TCP, tcp_rcv,
		net_proto_handle_error_none);
EMBOX_NET_PROTO(ETH_P_IPV6, IPPROTO_TCP, tcp_rcv,
		net_proto_handle_error_none);

#define MODOPS_VERIFY_CHKSUM OPTION_GET(BOOLEAN, verify_chksum)
#define TCP_FINWAIT2_TIMEOUT OPTION_GET(NUMBER, tcp_finwait2_timeout_ms)

#if OPTION_GET(STRING, log_level) >= LOG_DEBUG
#define TCP_DEBUG 1
#else
#define TCP_DEBUG 0
#endif

/* set to 1 to see all the changing states of tcp sockets */
#define TCP_PRINT_STATE_INFO 0

/** TODO
 * +1. Create default socket for resetting
 * +2. PSH flag
 * +3. RST flag
 * ?4. Changes state's logic (i.e. TCP_CLOSED for all socket which doesn't exists etc.)
 * +5. Rewrite send_from_sock (don't send new skb if queue is not empty)
 * +6. tcp_sock_free in tcp_st_finwait_2
 * +7. Remove seq_next (use rem.seq instead, build packet, and then rebuild only)
 * +8. Add lock/unlock
 * +9. Add rexmit
 * +10. Add window
 * +11. Add options
 * +12. Add timeout (i.e. sockopts) to tcp_sock.c
 */

/* Error code of TCP handlers */
enum tcp_ret_code {
	TCP_RET_OK,       /* all ok, don't free packet */
	TCP_RET_DROP,     /* drop packet */
	TCP_RET_SEND_SEQ, /* send packet */
	TCP_RET_SEND,     /* send acknowledgment or other packet
						 without outgoing queue using OLD skb*/
	TCP_RET_SEND_ALLOC, /* send acknowledgment or other packet
						  without outgoing queue using NEW skb */
	TCP_RET_RST,      /* reset (only for pre_process) */
	TCP_RET_FREE      /* drop packet and free socket */
};

/* Type of TCP state handlers */
typedef enum tcp_ret_code (*tcp_handler_t)(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph);

static struct sys_timer tcp_tmr_default; /* Timer structure for rexmitting or TIME-WAIT satate */

static void tcp_timer_handler(struct sys_timer *timer, void *param);

/* Prototypes */
static int tcp_handle(struct tcp_sock *tcp_sk, struct sk_buff *skb, tcp_handler_t hnd);
static const tcp_handler_t tcp_st_handler[];
static void tcp_get_now(struct timeval *out_now);

/************************ Debug functions ******************************/
#if !TCP_DEBUG
static inline void packet_print(const struct tcp_sock *tcp_sk,
		const struct sk_buff *skb, const char *msg, int family,
		const void *addr, in_port_t port) { }

#else

static inline void packet_print(const struct tcp_sock *tcp_sk,
		const struct sk_buff *skb, const char *msg, int family,
		const void *addr, in_port_t port) {
	struct timeval now;
	char buff[INET6_ADDRSTRLEN];

	tcp_get_now(&now);
	log_debug("%ld.%ld %s:%d %s sk %p skb %p seq %u ack %u seq_len %u flags %s %s %s %s %s %s %s %s",
			/* info */
			now.tv_sec, now.tv_usec,
			inet_ntop(family, addr, &buff[0], sizeof buff),
			ntohs(port), msg,
			tcp_sk != NULL ? to_sock(tcp_sk) : NULL, skb,
			/* seq, ack, seq_len */
			ntohl(skb->h.th->seq), ntohl(skb->h.th->ack_seq), tcp_seq_length(skb->h.th, skb->nh.raw),
			/* flags */
			(skb->h.th->ack ? "ACK" : ""), (skb->h.th->syn ? "SYN" : ""),
			(skb->h.th->fin ? "FIN" : ""), (skb->h.th->rst ? "RST" : ""),
			(skb->h.th->psh ? "PSH" : ""), (skb->h.th->urg ? "URG" : ""),
			(skb->h.th->ece ? "ECE" : ""), (skb->h.th->cwr ? "CWR" : ""));
}

#endif

/************************ Auxiliary functions **************************/
int alloc_prep_skb(struct tcp_sock *tcp_sk, size_t opt_len,
		size_t *data_len, struct sk_buff **out_skb) {
	int ret;
	struct sock *sk;
	size_t total_len, hdr_len;

	assert(tcp_sk != NULL);
	assert(out_skb != NULL);

	sk = to_sock(tcp_sk);
	if (sk->o_ops == NULL) {
		return -ENOSYS;
	}

	opt_len = (opt_len + 3) & ~(size_t)3;
	hdr_len = TCP_MIN_HEADER_SIZE + opt_len;
	total_len = hdr_len + (data_len != NULL ? *data_len : 0);

	assert(sk->o_ops->make_pack != NULL);
	ret = sk->o_ops->make_pack(sk, NULL, &total_len, out_skb);
	if (ret != 0) {
		return ret;
	}
	else if (total_len < hdr_len + (data_len != NULL)) {
		skb_free(*out_skb);
		return -EMSGSIZE;
	}

	if (data_len != NULL) {
		*data_len = total_len - hdr_len;
	}

	return 0;
}

void tcp_sock_lock(struct tcp_sock *tcp_sk, unsigned int obj) {
	if (tcp_sk->lock++ == 0) {
		sched_lock();
	}
}

void tcp_sock_unlock(struct tcp_sock *tcp_sk, unsigned int obj) {
	assert(tcp_sk->lock != 0);
	if (--tcp_sk->lock == 0) {
		sched_unlock();
	}
}

void tcp_seq_state_set_wind_value(struct tcp_seq_state *tcp_seq_st,
		uint16_t value) {
	if (tcp_seq_st->wind.value != value) {
		tcp_seq_st->wind.value = value;
		tcp_seq_st->wind.size = value << tcp_seq_st->wind.factor;
	}
}

void tcp_seq_state_set_wind_factor(struct tcp_seq_state *tcp_seq_st,
		uint8_t factor) {
	if (tcp_seq_st->wind.factor != factor) {
		tcp_seq_st->wind.factor = factor;
		tcp_seq_st->wind.size = tcp_seq_st->wind.value << factor;
	}
}

static void tcp_sock_rcv(struct tcp_sock *tcp_sk,
		struct sk_buff *skb) {
	size_t seq_off;

	assert(tcp_sk != NULL);
	assert(skb != NULL);
	assert(tcp_sk->rem.seq >= ntohl(skb->h.th->seq)); /* FIXME */
	seq_off = tcp_sk->rem.seq - ntohl(skb->h.th->seq);

	assert(tcp_data_length(skb->h.th, skb->nh.raw) > seq_off);
	sock_rcv(to_sock(tcp_sk), skb, skb->h.raw
			+ TCP_HEADER_SIZE(skb->h.th) + seq_off,
			tcp_data_length(skb->h.th, skb->nh.raw) - seq_off);
}

static void tcp_timer_update(void) {
	bool enable_tcp_timer = false;
	struct sock *sk;
	struct tcp_sock *tcp_sk;

	sock_foreach(sk, tcp_sock_ops) {
		tcp_sk = to_tcp_sock(sk);
		if ((tcp_sock_get_status(tcp_sk) != TCP_ST_NOTEXIST)) {
			enable_tcp_timer = true;
			break;
		}
	}

	if (enable_tcp_timer) {
		if (!timer_is_started(&tcp_tmr_default)) {
			timer_init_start_msec(&tcp_tmr_default, TIMER_PERIODIC,
					TCP_TIMER_FREQUENCY, tcp_timer_handler, NULL);
		}
	} else {
		if (timer_is_started(&tcp_tmr_default)) {
			timer_stop(&tcp_tmr_default);
		}
	}
}

void tcp_sock_set_state(struct tcp_sock *tcp_sk,
		enum tcp_sock_state new_state) {
	const char *str_state[TCP_MAX_STATE] = {"TCP_CLOSED",
			"TCP_LISTEN",
			"TCP_SYN_SENT",
			"TCP_SYN_RECV_PRE",
			"TCP_SYN_RECV",
			"TCP_ESTABIL",
			"TCP_FINWAIT_1",
			"TCP_FINWAIT_2",
			"TCP_CLOSEWAIT",
			"TCP_CLOSING",
			"TCP_LASTACK",
			"TCP_TIMEWAIT"
	};
	struct sock *sk = to_sock(tcp_sk);

	(void)str_state; /* This var may not be used if max_log_level < LOG_DEBUG */

#if TCP_PRINT_STATE_INFO
    printk("tcp_sk = 0x%08x\t set state %s\n", (uintptr_t)tcp_sk,
            str_state[new_state]);
#endif

	switch (new_state) {
	default:
		break;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
		tcp_get_now(&tcp_sk->syn_time); /* set when SYN sent */
		/* fallthrough */
	case TCP_FINWAIT_1:
	case TCP_LASTACK:
		tcp_sk->ack_flag = tcp_sk->self.seq + 1;
		log_debug("sk %p set ack_flag %u for state %d-%s",
				sk, tcp_sk->ack_flag, new_state, str_state[new_state]);
		break;
	}

	tcp_timer_update();

	tcp_sk->state = new_state;
	log_debug("sk %p set state %d-%s", sk, new_state, str_state[new_state]);

	/* idesc manipulation */
	switch (new_state) {
	default:
		break;
	case TCP_ESTABIL: /* new connection */
		/* enable writing when connection is established */
		sock_notify(sk, POLLOUT); /* FIXME tcp_sock was notified earlier at line 911 */
		/* enable reading for listening (parent) socket */
		if (tcp_sk->parent != NULL) {
			tcp_sock_lock(tcp_sk->parent, TCP_SYNC_CONN_QUEUE);
			{
				list_move_tail(&tcp_sk->conn_lnk, &tcp_sk->parent->conn_ready);
			}
			tcp_sock_unlock(tcp_sk->parent, TCP_SYNC_CONN_QUEUE);
			assert(to_sock(tcp_sk->parent) != NULL);

			//FIXME tcp_accept must notify without rx_data_len
			to_sock(tcp_sk->parent)->rx_data_len++;
			sock_notify(to_sock(tcp_sk->parent), POLLIN);
		}
		break;
	case TCP_CLOSEWAIT: /* throw error: can't read */
		sock_update_err(sk, ECONNRESET);
		sock_set_so_error(sk, 1);
		sock_notify(sk, POLLIN | POLLERR);
		break;
	case TCP_TIMEWAIT: /* throw error: can't read and write */
	case TCP_CLOSING:
	case TCP_CLOSED:
		sock_update_err(sk, ECONNRESET);
		sock_set_so_error(sk, 1);
		sock_notify(sk, POLLIN | POLLOUT | POLLERR);
		break;
	}
}

int tcp_sock_get_status(struct tcp_sock *tcp_sk) {
	assert(tcp_sk != NULL);

	switch (tcp_sk->state) {
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

static void tcp_get_now(struct timeval *out_now) {
	ktime_get_timeval(out_now);
}

static int tcp_is_expired(struct timeval *since, useconds_t limit_msec) {
	struct timeval now, delta, limit;

	ktime_get_timeval(&now);
	timersub(&now, since, &delta);
	limit.tv_sec = limit_msec / MSEC_PER_SEC;
	limit.tv_usec = (limit_msec % MSEC_PER_SEC) * USEC_PER_MSEC;

	return timercmp(&delta, &limit, >=);
}

static void tcp_xmit(struct sk_buff *skb,
		const struct tcp_sock *tcp_sk,
		const struct net_pack_out_ops *out_ops) {
	assert(skb != NULL);

	packet_print(tcp_sk, skb, "<=",
			ip_hdr(skb)->version == 4 ? AF_INET : AF_INET6,
			ip_hdr(skb)->version == 4 ?
					(void *)&ip_hdr(skb)->daddr : (void *)&ip6_hdr(skb)->daddr,
			tcp_hdr(skb)->dest);

	out_ops = out_ops != NULL ? out_ops :
			tcp_sk != NULL ? to_sock(tcp_sk)->o_ops : NULL;

	if (out_ops != NULL) {
		int ret = out_ops->snd_pack(skb);
		if (ret != 0) {
			log_debug("snd_pack = %d", ret);
		}
	}
}

static void tcp_rexmit(struct tcp_sock *tcp_sk) {
	struct sk_buff *skb, *skb_send;

	tcp_sock_lock(tcp_sk, TCP_SYNC_WRITE_QUEUE);
	{
		skb = skb_queue_front(&to_sock(tcp_sk)->tx_queue);
		if (skb == NULL) {
			/**
			 * TODO
			 * self.seq is set in the function up the stack,
			 * so assert may trigger falsely. Need add lock
			 * and after that it will be correct.
			 */
			/* assert(sock.tcp_sk->last_ack == sock.tcp_sk->self.seq); */
			tcp_sock_unlock(tcp_sk, TCP_SYNC_WRITE_QUEUE);
			return;
		}
		skb_send = skb_clone(skb);
		if (skb_send == NULL) {
			tcp_sock_unlock(tcp_sk, TCP_SYNC_WRITE_QUEUE);
			return;
		}
		log_debug("send skb %p, postponed %p", skb_send, skb);
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_WRITE_QUEUE);

	tcp_xmit(skb_send, tcp_sk, NULL);
}

static void send_rst_reply(struct sk_buff *skb) {
	struct tcphdr old_tcph, *tcph;
	size_t tcph_size, old_seq_len;
	const struct net_pack_out_ops *out_ops;

	memcpy(&old_tcph, tcp_hdr(skb), sizeof old_tcph);
	old_seq_len = tcp_seq_length(&old_tcph, skb->nh.raw);
	tcph_size = TCP_MIN_HEADER_SIZE;

	out_ops = ip_hdr(skb)->version == 4 ? ip_out_ops : ip6_out_ops;
	if (out_ops == NULL) {
		return; /* error: not implemented */
	}

	/* make packet with L3 header */
	assert(out_ops->make_pack != NULL);
	if (0 != out_ops->make_pack(NULL, NULL, &tcph_size, &skb)) {
		return; /* error: see ret */
	}
	else if (tcph_size < TCP_MIN_HEADER_SIZE) {
		skb_free(skb);
		return; /* error: no memory */
	}

	/* build TCP header */
	tcph = tcp_hdr(skb);
	tcp_build(tcph, old_tcph.source, old_tcph.dest, TCP_MIN_HEADER_SIZE, 0);

	tcph->rst = 1;
	if (old_tcph.ack) {
		tcp_set_seq_field(tcph, ntohl(old_tcph.ack_seq));
	}
	else {
		tcp_set_seq_field(tcph, 0);
		tcp_set_ack_field(tcph, ntohl(old_tcph.seq) + old_seq_len);
	}
	tcp_set_check_field(tcph, skb->nh.raw);

	/* send over L3 */
	tcp_xmit(skb, NULL, out_ops);
}

/**
 * Send any packet without sequence (i.e. seq_len is 0)
 */
static void send_nonseq_from_sock(struct tcp_sock *tcp_sk,
		struct sk_buff *skb) {
	log_debug("send %p", skb);
	tcp_set_seq_field(skb->h.th, tcp_sk->self.seq);
	tcp_set_check_field(skb->h.th, skb->nh.raw);
	tcp_xmit(skb, tcp_sk, NULL);
}

/**
 * Send a data, only
 */
void send_seq_from_sock(struct tcp_sock *tcp_sk, struct sk_buff *skb) {
	struct sk_buff *skb_send;

	assert(tcp_sk != NULL);
	assert(skb != NULL);

	skb_send = skb_clone(skb);

	log_debug("send %p = %p", skb, skb_send);

	tcp_sock_lock(tcp_sk, TCP_SYNC_WRITE_QUEUE);
	{
		tcp_set_seq_field(skb->h.th, tcp_sk->self.seq);
		tcp_set_check_field(skb->h.th, skb->nh.raw);
		if (skb_send != NULL) {
			/* set to cloned pkg */
			memcpy(skb_send->h.th, skb->h.th, sizeof *skb->h.th);
		}
		assert(to_sock(tcp_sk) != NULL);
		skb_queue_push(&to_sock(tcp_sk)->tx_queue, skb);
		tcp_sk->self.seq += tcp_seq_length(skb->h.th, skb->nh.raw);
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_WRITE_QUEUE);

	if (skb_send != NULL) {
		tcp_xmit(skb_send, tcp_sk, NULL);
	}
}

/* Helper to determine if the entry is in specific list */
static inline int is_in_list(struct list_head *entry, struct list_head *head) {
    struct list_head *pos;

    list_for_each(pos, head) {
        if (pos == entry)
            return 1;
    }

    return 0;
}

void tcp_sock_release(struct tcp_sock *tcp_sk) {
	struct tcp_sock *anticipant;

	if (tcp_sk->parent == NULL) {
		tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
		{
			list_for_each_entry(anticipant,
					&tcp_sk->listening.conn_wait, conn_lnk) {
				sock_release(to_sock(anticipant));
			}
			list_for_each_entry(anticipant, &tcp_sk->conn_ready, conn_lnk) {
				sock_release(to_sock(anticipant));
			}
			list_for_each_entry(anticipant, &tcp_sk->listening.conn_free, conn_lnk) {
				sock_release(to_sock(anticipant));
			}
		}
		tcp_sock_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);
	}
	else {
        /*If this is not a parent socket => it is either a socket that was at
         * established connection or it was for some reason dropped from the
         * waiting connections queue.*/
		tcp_sock_lock(tcp_sk->parent, TCP_SYNC_CONN_QUEUE);
        {
            /* If it is in the queue */
            if (!list_empty(&tcp_sk->conn_lnk)) {
                /* Doesn't seem possible, but if that's a socket from conn_free
                 * queue we need to decrease listening socket's wait_queue_len*/
                if(is_in_list(&tcp_sk->conn_lnk, &tcp_sk->parent->listening.conn_free)) {
                    assert(tcp_sk->parent->listening.wait_queue_len > 0);
                    tcp_sk->parent->listening.wait_queue_len--;
                }
                list_del(&tcp_sk->conn_lnk);
            }
        }
		tcp_sock_unlock(tcp_sk->parent, TCP_SYNC_CONN_QUEUE);
	}

	sock_release(to_sock(tcp_sk));

	tcp_timer_update();
}


/****************** Handlers of TCP states ***********************/
static enum tcp_ret_code tcp_st_closed(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_closed");
	assert(tcp_sk->state == TCP_CLOSED);

	return TCP_RET_RST;
}

/* From tcp_sock layer */
extern int tcp_sock_listen_alloc(struct tcp_sock *tcp_sk);
extern struct tcp_sock *tcp_sock_listen_fetch(struct tcp_sock *tcp_s);

static enum tcp_ret_code tcp_st_listen(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	union {
		struct inet_sock *in;
		struct inet6_sock *in6;
	} newsk;
	struct tcp_sock *tcp_newsk;

	log_debug("call tcp_st_listen");
	assert(tcp_sk->state == TCP_LISTEN);

	if (tcph->syn) {
		/* Check max length of accept queue and reserve 1 place */
		tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
		{
            if (tcp_sock_listen_alloc(tcp_sk) > 0) {
                tcp_newsk = tcp_sock_listen_fetch(tcp_sk); /* will add it to conn_wait list */
			} else {
				log_error("tcp_st_listen: can't alloc socket\n");
				tcp_newsk = NULL;
				/* maybe calling thread could allocate more? */
				sock_notify(to_sock(tcp_sk), POLLIN);
			}
		}
		tcp_sock_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);

		if (!tcp_newsk) {
			return TCP_RET_DROP; /* error: see ret */
		}

		log_debug("append sk %p for skb %p to sk %p queue",
				to_sock(tcp_newsk), skb, to_sock(tcp_sk));
		/* Set up new socket */
		if (to_sock(tcp_sk)->opt.so_domain == AF_INET) {
			assert(ip_hdr(skb)->version == 4);
			newsk.in = to_inet_sock(to_sock(tcp_newsk));
			newsk.in->src_in.sin_family = AF_INET;
			newsk.in->src_in.sin_port = tcph->dest;
			memcpy(&newsk.in->src_in.sin_addr,
					&ip_hdr(skb)->daddr,
					sizeof newsk.in->src_in.sin_addr);
			newsk.in->dst_in.sin_family = AF_INET;
			newsk.in->dst_in.sin_port = tcph->source;
			memcpy(&newsk.in->dst_in.sin_addr,
					&ip_hdr(skb)->saddr,
					sizeof newsk.in->dst_in.sin_addr);
		}
		else {
			assert(to_sock(tcp_sk)->opt.so_domain == AF_INET6);
			assert(ip6_hdr(skb)->version == 6);
			newsk.in6 = to_inet6_sock(to_sock(tcp_newsk));
			newsk.in6->src_in6.sin6_family = AF_INET6;
			newsk.in6->src_in6.sin6_port = tcph->dest;
			memcpy(&newsk.in6->src_in6.sin6_addr,
					&ip6_hdr(skb)->daddr,
					sizeof newsk.in6->src_in6.sin6_addr);
			newsk.in6->dst_in6.sin6_family = AF_INET6;
			newsk.in6->dst_in6.sin6_port = tcph->source;
			memcpy(&newsk.in6->dst_in6.sin6_addr,
					&ip6_hdr(skb)->saddr,
					sizeof newsk.in6->dst_in6.sin6_addr);
		}
		/* Save new socket to accept queue */
		tcp_sock_lock(tcp_sk, TCP_SYNC_CONN_QUEUE);
		{
			tcp_newsk->parent = tcp_sk;
			list_add_tail(&tcp_newsk->conn_lnk, &tcp_sk->listening.conn_wait);
		}
		tcp_sock_unlock(tcp_sk, TCP_SYNC_CONN_QUEUE);
		/* Handling skb */
		tcp_sock_lock(tcp_sk, TCP_SYNC_STATE); /* FIXME */
		{
			tcp_sock_set_state(tcp_newsk, TCP_SYN_RECV_PRE); /* TODO remove this */
			tcp_handle(tcp_newsk, skb, tcp_st_handler[TCP_SYN_RECV_PRE]);
		}
		tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

		return TCP_RET_OK;
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_syn_sent(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_syn_sent");
	assert(tcp_sk->state == TCP_SYN_SENT);

	if (tcph->syn) {
		tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
		tcp_seq_state_set_wind_value(&tcp_sk->rem,
				ntohs(tcph->window));
		if (tcph->ack) {
			tcp_sock_set_state(tcp_sk, TCP_ESTABIL);
		} else {
			tcp_sock_set_state(tcp_sk, TCP_SYN_RECV);
		}
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND;
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_syn_recv_pre(
		struct tcp_sock *tcp_sk, const struct tcphdr *tcph,
		struct sk_buff *skb, struct tcphdr *out_tcph) {
	log_debug("call tcp_st_syn_recv_pre");
	assert(tcp_sk->state == TCP_SYN_RECV_PRE);

	if (tcph->syn) {
		tcp_sk->rem.seq = ntohl(tcph->seq) + 1;
		tcp_seq_state_set_wind_value(&tcp_sk->rem,
				ntohs(tcph->window));
		tcp_sock_set_state(tcp_sk, TCP_SYN_RECV);
		out_tcph->syn = 1;
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND_SEQ;
	}
	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_syn_recv(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_syn_recv");
	assert(tcp_sk->state == TCP_SYN_RECV);

	if (tcph->ack) {
		tcp_sock_set_state(tcp_sk, TCP_ESTABIL);
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_estabil(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	size_t data_len;

	log_debug("call tcp_st_estabil");
	assert(tcp_sk->state == TCP_ESTABIL);

	data_len = tcp_data_length(skb->h.th, skb->nh.raw);
	if (data_len > 0) {
		/* Save current sk_buff_t with data */
		log_debug("\t received %d", data_len);
		tcp_sock_rcv(tcp_sk, skb);
		tcp_sk->rem.seq += data_len;
		if (tcph->fin) {
			tcp_sk->rem.seq += 1;
			tcp_sock_set_state(tcp_sk, TCP_CLOSEWAIT);
		}
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND_ALLOC;
	} else if (tcph->fin) {
		tcp_sk->rem.seq += 1;
		tcp_sock_set_state(tcp_sk, TCP_CLOSEWAIT);
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND;
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_finwait_1(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	size_t data_len;

	log_debug("call tcp_st_finwait_1");
	assert(tcp_sk->state == TCP_FINWAIT_1);

	data_len = tcp_data_length(skb->h.th, skb->nh.raw);
	if (data_len > 0) {
		/* Save current sk_buff_t with data */
		log_debug("\t received %d", data_len);
		tcp_sock_rcv(tcp_sk, skb);
		tcp_sk->rem.seq += data_len;
		if (tcph->fin) {
			tcp_sk->rem.seq += 1;
			if (tcph->ack) {
				tcp_sock_set_state(tcp_sk, TCP_TIMEWAIT);
			} else {
				tcp_sock_set_state(tcp_sk, TCP_CLOSING);
			}
		} else if (tcph->ack) {
			tcp_sock_set_state(tcp_sk, TCP_FINWAIT_2);
		}
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND_ALLOC;
	} else if (tcph->fin) {
		tcp_sk->rem.seq += 1;
		if (tcph->ack) {
			tcp_sock_set_state(tcp_sk, TCP_TIMEWAIT);
		} else {
			tcp_sock_set_state(tcp_sk, TCP_CLOSING);
		}
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND;
	} else if (tcph->ack) {
		tcp_sock_set_state(tcp_sk, TCP_FINWAIT_2);
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_finwait_2(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	size_t data_len;

	log_debug("call tcp_st_finwait_2");
	assert(tcp_sk->state == TCP_FINWAIT_2);

	data_len = tcp_data_length(skb->h.th, skb->nh.raw);
	if (data_len > 0) {
		/* Save current sk_buff_t with data */
		log_debug("\t received %d\n", data_len);
		tcp_sock_rcv(tcp_sk, skb);
		tcp_sk->rem.seq += data_len;
		if (tcph->fin) {
			tcp_sk->rem.seq += 1;
			tcp_sock_set_state(tcp_sk, TCP_TIMEWAIT);
		}
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND_ALLOC;
	} else if (tcph->fin) {
		tcp_sk->rem.seq += 1;
		tcp_sock_set_state(tcp_sk, TCP_TIMEWAIT);
		tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
		return TCP_RET_SEND;
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_closewait(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_closewait");
	assert(tcp_sk->state == TCP_CLOSEWAIT);

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_closing(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_closing");
	assert(tcp_sk->state == TCP_CLOSING);

	if (tcph->ack) {
		tcp_sock_set_state(tcp_sk, TCP_TIMEWAIT);
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_lastack(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_lastack");
	assert(tcp_sk->state == TCP_LASTACK);

	if (tcph->ack) {
		tcp_sock_set_state(tcp_sk, TCP_CLOSED);
		return TCP_RET_FREE;
	}

	return TCP_RET_DROP;
}

static enum tcp_ret_code tcp_st_timewait(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	log_debug("call tcp_st_timewait");
	assert(tcp_sk->state == TCP_TIMEWAIT);

	/* timeout 2msl and set TCP_CLOSED state */

	return TCP_RET_DROP;
}


/************************ Process functions ****************************/
static enum tcp_ret_code process_rst(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph) {
	switch (tcp_sk->state) {
	default:
		break;
	case TCP_LASTACK:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_TIMEWAIT: /* don't wait for tcp_timer to collect
						  the socket */
		/* socket have a state only after close call.
		 * It could be freed only here */
		return TCP_RET_FREE;
	case TCP_SYN_SENT:
		if (tcp_sk->self.seq != ntohl(tcph->ack_seq)) {
			/* invalid reset */
			break;
		}
		/* PASSTHROUGH */
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
		tcp_sock_set_state(tcp_sk, TCP_CLOSED);
		if (!list_empty(&tcp_sk->conn_lnk)) {
			assert(tcp_sk->parent != NULL);
			return TCP_RET_FREE;
		}
		break;
	}

	return TCP_RET_DROP;
}

static void confirm_ack(struct tcp_sock *tcp_sk, uint32_t ack) {
	struct sk_buff *sent_skb;
	uint32_t ack2seq, seq_len;

	log_debug("sk %p ack %u", to_sock(tcp_sk), ack);
	tcp_sock_lock(tcp_sk, TCP_SYNC_WRITE_QUEUE);
	{
		do {
			sent_skb = skb_queue_front(&to_sock(tcp_sk)->tx_queue);
			assert(sent_skb != NULL);
			ack2seq = ack - ntohl(sent_skb->h.th->seq);
			seq_len = tcp_seq_length(sent_skb->h.th,
					sent_skb->nh.raw);
			if (ack2seq >= seq_len) {
				log_debug("confirm_ack: remove skb %p\n",
						sent_skb);
				skb_free(sent_skb); /* list_del_init will done
									   at skb_free */
			}
		} while (ack2seq > seq_len);
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_WRITE_QUEUE);
}

static enum tcp_ret_code process_ack(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph) {
	uint32_t ack, ack2last_ack, seq;

	/* Resetting if recv ack in this state */
	switch (tcp_sk->state) {
	default:
		break;
	case TCP_CLOSED:
	case TCP_LISTEN:
		return TCP_RET_RST;
	}

	ack = ntohl(tcph->ack_seq);
	ack2last_ack = ack - tcp_sk->last_ack;
	seq = tcp_sk->self.seq;

	if (ack2last_ack == 0) {
		/* no new acknowledgments */
		if ((seq != ack) && !tcp_sk->rexmit_mode) {
			++tcp_sk->dup_ack;
			if (tcp_sk->dup_ack == TCP_REXMIT_DUP_ACK) {
				tcp_sk->rexmit_mode = 1;
				tcp_rexmit(tcp_sk);
			}
		}
	}
	else if (ack2last_ack <= seq - tcp_sk->last_ack) {
		confirm_ack(tcp_sk, ack);
		tcp_sk->last_ack = ack;
		tcp_get_now(&tcp_sk->ack_time);
		if (!tcp_sk->rexmit_mode) {
			tcp_sk->dup_ack = 0;
			sock_notify(to_sock(tcp_sk), POLLOUT);
		}
		else {
			if (seq == ack) {
				tcp_sk->rexmit_mode = 0;
				tcp_sk->dup_ack = 0;
				sock_notify(to_sock(tcp_sk), POLLOUT);
			}
			else {
				tcp_rexmit(tcp_sk);
			}
		}
	}
	else if (ack - seq <= ack2last_ack) {
		/* package with non-last acknowledgment */
	}
	else {
		assert(seq - tcp_sk->last_ack < ack2last_ack);
		/* assert(ack > tcp_sk->self.seq); -- without overflow checks */
		log_debug("invalid acknowledgments: last_ack=%u ack=%u seq=%u",
				tcp_sk->last_ack, ack, seq);
		switch (tcp_sk->state) {
		default:
			break;
		case TCP_SYN_SENT:
		case TCP_SYN_RECV_PRE:
		case TCP_SYN_RECV:
			return TCP_RET_RST;
		}
		return TCP_RET_DROP;
	}

	/* Check ack flag for our state */
	switch (tcp_sk->state) {
	default:
		break;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
	case TCP_FINWAIT_1:
	case TCP_CLOSING:
	case TCP_LASTACK:
		if (ack - tcp_sk->ack_flag <= seq - tcp_sk->ack_flag) {
			/* All ok, our flag was confirmed */
		}
		else { /* Else unmark ack flag */
			log_debug("sk %p unmark ack\n",
					to_sock(tcp_sk));
			/* XXX remove const qualifier */
			((struct tcphdr *)tcph)->ack = 0;
		}
		break;
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
}
#endif

static enum tcp_ret_code process_opt(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph) {
	char *ptr = (char *)&tcph->options[0];
	char *end = ptr + TCP_HEADER_SIZE(tcph) - TCP_MIN_HEADER_SIZE;
	char *prev_ptr;

	do {
		prev_ptr = ptr;
		switch (*ptr) {
		default:
			ptr += *(ptr + 1);
			break;
		case TCP_OPT_KIND_NOP:
			++ptr;
			break;
		case TCP_OPT_KIND_WS:
			if (*(ptr + 1) == 3) {
				tcp_seq_state_set_wind_factor(&tcp_sk->rem,
						*(ptr + 2));
			}
			ptr += *(ptr + 1);
			break;
		}
		// TODO this is hack to fix cycling when ptr does not change
		if (prev_ptr == ptr) {
			++ptr;
		}
	} while (ptr < end);

	// TODO revert this comment
	//assert(ptr == end);

	return TCP_RET_OK;
}

static enum tcp_ret_code pre_process(struct tcp_sock *tcp_sk,
		const struct tcphdr *tcph, struct sk_buff *skb,
		struct tcphdr *out_tcph) {
	int ret;
	uint32_t seq2rem_seq, seq_len, seq_last2rem_seq, rem_len;

	/* Check CRC */
	if (MODOPS_VERIFY_CHKSUM) {
		uint16_t old_check;
		old_check = tcph->check;
		/* XXX remove const qualifier */
		tcp_set_check_field((struct tcphdr *)tcph, skb->nh.raw);
		if (old_check != tcph->check) {
			log_error("invalid checksum %hx(%hx) sk %p skb %p",
						ntohs(old_check), ntohs(tcph->check),
						to_sock(tcp_sk), skb);
			return TCP_RET_DROP;
		}
	}

	/* Process RST */
	if (tcph->rst) {
		ret = process_rst(tcp_sk, tcph);
		if (ret != TCP_RET_OK) {
			return ret;
		}
	}

	/* Analyze sequence */
	switch (tcp_sk->state) {
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
		seq2rem_seq = ntohl(tcph->seq) - tcp_sk->rem.seq;
		seq_len = tcp_seq_length(skb->h.th, skb->nh.raw);;
		seq_last2rem_seq = seq2rem_seq + seq_len;
		rem_len = tcp_sk->self.wind.size;
		if (seq2rem_seq < rem_len) {
			if (seq2rem_seq != 0) {
				/* TODO There is correct packet (with
				 * correct sequence number), but some packages
				 * was lost. We should save this skb, and wait
				 * previous packages.
				 */
				return TCP_RET_DROP;
			}
		}
		else if ((seq_last2rem_seq != 0)
				&& (seq_last2rem_seq <= rem_len)) { }
		else {
			log_debug("received old package:"
						" rem_seq=%u seq=%u seq_last=%u rem_last=%u\n",
					tcp_sk->rem.seq, ntohl(tcph->seq),
					ntohl(tcph->seq) + seq_len,
					tcp_sk->rem.seq + rem_len);
			if (seq2rem_seq <= -seq_len) {
				/* Send segment with ack flag if this packet
				 * is duplicated */
				tcp_set_ack_field(out_tcph, tcp_sk->rem.seq);
				return TCP_RET_SEND;
			}
			return TCP_RET_DROP;
		}
		break;
	}

	/* Porcess ACK */
	if (tcph->ack) {
		ret = process_ack(tcp_sk, tcph);
		if (ret != TCP_RET_OK) {
			return ret;
		}
	}

	/* Update window */
	switch (tcp_sock_get_status(tcp_sk)) {
	default:
		break;
	case TCP_ST_SYNC:
		tcp_seq_state_set_wind_value(&tcp_sk->rem,
				ntohs(tcph->window));
		break;
	}

	/* Process options */
	if (TCP_HEADER_SIZE(tcph) != TCP_MIN_HEADER_SIZE) {
		ret = process_opt(tcp_sk, tcph);
		if (ret != TCP_RET_OK) {
			return ret;
		}
	}

	return TCP_RET_OK;
}

/************************ Handlers table *******************************/
static const tcp_handler_t tcp_st_handler[TCP_MAX_STATE] = {
		[ TCP_CLOSED ] = tcp_st_closed,
		[ TCP_LISTEN ] = tcp_st_listen,
		[ TCP_SYN_SENT ] = tcp_st_syn_sent,
		[ TCP_SYN_RECV_PRE ] = tcp_st_syn_recv_pre,
		[ TCP_SYN_RECV ] = tcp_st_syn_recv,
		[ TCP_ESTABIL ] = tcp_st_estabil,
		[ TCP_FINWAIT_1 ] = tcp_st_finwait_1,
		[ TCP_FINWAIT_2 ] = tcp_st_finwait_2,
		[ TCP_CLOSEWAIT ] = tcp_st_closewait,
		[ TCP_CLOSING ] = tcp_st_closing,
		[ TCP_LASTACK ] = tcp_st_lastack,
		[ TCP_TIMEWAIT ] = tcp_st_timewait
};

static int tcp_handle(struct tcp_sock *tcp_sk, struct sk_buff *skb,
		tcp_handler_t hnd) {
	/* If result is not TCP_RET_OK then further processing
	 * can't be made */
	enum tcp_ret_code ret;
	struct tcphdr out_tcph;
	struct sk_buff *out_skb;

	tcp_build(&out_tcph, skb->h.th->source, skb->h.th->dest,
			TCP_MIN_HEADER_SIZE, tcp_sk->self.wind.value);
	out_skb = NULL;

	/**
	 * If we want to use the current package (to save for example),
	 * we must create a new package to send a response (if we need
	 * to send a response). After this the handler may do anything
	 * with current sk_buff_t.
	 * If hnd is NULL we use synchronization tools.
	 */
	assert(tcp_sk->state < TCP_MAX_STATE);
	assert(tcp_st_handler[tcp_sk->state] != NULL);
	hnd = hnd != NULL ? hnd : tcp_st_handler[tcp_sk->state];

	tcp_sock_lock(tcp_sk, TCP_SYNC_STATE);
	{
		ret = hnd(tcp_sk, skb->h.th, skb, &out_tcph);
	}
	tcp_sock_unlock(tcp_sk, TCP_SYNC_STATE);

	log_debug("ret %d skb %p sk %p",
			ret,
			out_skb != NULL ? out_skb : skb,
			to_sock(tcp_sk));

	switch (ret) {
	case TCP_RET_FREE:
		/* skb may be listed in sock, so they must be free exactly
 		 * in this order */
		skb_free(skb);
		tcp_sock_release(tcp_sk);
		break;
	case TCP_RET_DROP:
		skb_free(skb);
		break;
	case TCP_RET_SEND_SEQ:
	case TCP_RET_SEND:
		if (NULL == skb_declone(skb)) {
			skb_free(skb);
			return TCP_RET_DROP; /* error: ENOMEM */
		}
		/* fallthrough */
	case TCP_RET_SEND_ALLOC:
		out_skb = ret != TCP_RET_SEND_ALLOC ? skb : NULL;
		if (0 != alloc_prep_skb(tcp_sk, 0, NULL, &out_skb)) {
			return TCP_RET_DROP; /* error: see ret */
		}
		memcpy(out_skb->h.th, &out_tcph, sizeof out_tcph);
		if (ret == TCP_RET_SEND_SEQ) {
			send_seq_from_sock(tcp_sk, out_skb);
		}
		else {
			send_nonseq_from_sock(tcp_sk, out_skb);
		}
		break;
	case TCP_RET_RST: /* this processing in tcp_process */
	case TCP_RET_OK:
		break;
	}

	return ret;
}

/**
 * Main function of TCP protocol
 */
static void tcp_process(struct tcp_sock *tcp_sk, struct sk_buff *skb) {

	if (tcp_sk != NULL) {
		enum tcp_ret_code ret;

		tcp_get_now(&tcp_sk->rcv_time);

		ret = tcp_handle(tcp_sk, skb, pre_process);
		if (ret == TCP_RET_OK) {
			ret = tcp_handle(tcp_sk, skb, NULL);
		}

		if (ret == TCP_RET_RST) {
			send_rst_reply(skb);
		}
	}
	else if (tcp_hdr(skb)->rst) {
		/* ignore RST when socket doesn't exist */
		skb_free(skb);
	}
	else {
		/* generate RST when socket doesn't exist */
		send_rst_reply(skb);
	}
}

static int tcp4_rcv_tester_strict(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET)
			&& ip_tester_dst(sk, skb) && ip_tester_src(sk, skb)
			&& (sock_inet_get_src_port(sk) == tcp_hdr(skb)->dest)
			&& (sock_inet_get_dst_port(sk) == tcp_hdr(skb)->source);
};

static int tcp6_rcv_tester_strict(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET6)
			&& ip6_tester_dst(sk, skb) && ip6_tester_src(sk, skb)
			&& (sock_inet_get_src_port(sk) == tcp_hdr(skb)->dest)
			&& (sock_inet_get_dst_port(sk) == tcp_hdr(skb)->source);
};

static int tcp4_rcv_tester_soft(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET)
			&& ip_tester_dst_or_any(sk, skb)
			&& (sock_inet_get_src_port(sk) == tcp_hdr(skb)->dest)
			&& (sock_inet_get_dst_port(sk) == 0);
}

static int tcp6_rcv_tester_soft(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET6)
			&& ip6_tester_dst_or_any(sk, skb)
			&& (sock_inet_get_src_port(sk) == tcp_hdr(skb)->dest)
			&& (sock_inet_get_dst_port(sk) == 0);
}

extern uint16_t skb_get_secure_level(const struct sk_buff *skb);
extern uint16_t sock_get_secure_level(const struct sock *sk);

static int tcp_rcv_need_check_security(struct tcp_sock *tcp_sk) {
	switch (tcp_sk->state) {
	case TCP_CLOSED:
	case TCP_LISTEN:
		return 1;
	case TCP_SYN_SENT:
	case TCP_SYN_RECV_PRE:
	case TCP_SYN_RECV:
	case TCP_ESTABIL:
	case TCP_FINWAIT_1:
	case TCP_FINWAIT_2:
	case TCP_CLOSEWAIT:
	case TCP_CLOSING:
	case TCP_LASTACK:
	case TCP_TIMEWAIT:
	case TCP_MAX_STATE:
		break;
	}

	return 0;
}

static int tcp_rcv(struct sk_buff *skb) {
	struct sock *sk;
	struct tcp_sock *tcp_sk;

	assert(skb != NULL);
	assert(ip_check_version(ip_hdr(skb))
			|| ip6_check_version(ip6_hdr(skb)));

	sk = sock_lookup(NULL, tcp_sock_ops,
			ip_check_version(ip_hdr(skb))
				? tcp4_rcv_tester_strict
				: tcp6_rcv_tester_strict,
			skb);
	if (sk == NULL) {
		sk = sock_lookup(NULL, tcp_sock_ops,
				ip_check_version(ip_hdr(skb))
					? tcp4_rcv_tester_soft
					: tcp6_rcv_tester_soft,
				skb);
	}

	tcp_sk = sk != NULL ? to_tcp_sock(sk) : NULL;

	if (tcp_sk) {
		if (tcp_rcv_need_check_security(tcp_sk)) {
			/* if we have socket with secure label we have to check secure level */
			if (sock_get_secure_level(sk) >	skb_get_secure_level(skb)) {
				skb_free(skb);
				return 0;
			}
		}
	}

	packet_print(tcp_sk, skb, "=>",
			ip_check_version(ip_hdr(skb)) ? AF_INET : AF_INET6,
			ip_check_version(ip_hdr(skb))
				? (void *)&ip_hdr(skb)->saddr
				: (void *)&ip6_hdr(skb)->saddr,
			tcp_hdr(skb)->source);

	tcp_process(tcp_sk, skb);

	return 0;
}

static void tcp_timer_handler(struct sys_timer *timer, void *param) {
	struct sock *sk = NULL;
	struct tcp_sock *tcp_sk;

	(void)timer;
	(void)param;

	sock_foreach(sk, tcp_sock_ops) {
		tcp_sk = to_tcp_sock(sk);
        /* release TIMEWAIT socket after typically 2msl*/
		if ((tcp_sk->state == TCP_TIMEWAIT)
				&& tcp_is_expired(&tcp_sk->rcv_time, TCP_TIMEWAIT_DELAY)) {

			log_debug("release timewait sk %p", to_sock(tcp_sk));
			tcp_sock_release(tcp_sk);
		}
		else if ((tcp_sock_get_status(tcp_sk) == TCP_ST_NONSYNC)
				&& !list_empty(&tcp_sk->conn_lnk)
				&& tcp_is_expired(&tcp_sk->syn_time, TCP_SYNC_TIMEOUT)) {

			assert(tcp_sk->parent != NULL);
			log_debug("release nonsync sk %p", to_sock(tcp_sk));
			tcp_sock_release(tcp_sk);
		}
		else if ((tcp_sock_get_status(tcp_sk) != TCP_ST_NOTEXIST)
				&& tcp_is_expired(&tcp_sk->ack_time, TCP_REXMIT_DELAY)
				&& (tcp_sk->last_ack != tcp_sk->self.seq)) {

			log_debug("rexmit sk %p", to_sock(tcp_sk));
			tcp_sk->rexmit_mode = 1;
			tcp_rexmit(tcp_sk);
		}
        /* release socket resource wich is stuck in finwait2 state
         * violates specification, but seems reasonable (linux has it similar way)*/
        else {
            if (TCP_FINWAIT2_TIMEOUT != 0) {
                if ((tcp_sk->state == TCP_FINWAIT_2)
                        && tcp_is_expired(&tcp_sk->rcv_time, TCP_FINWAIT2_TIMEOUT)) {

                    log_debug("release finwait2 sk %p after %dms", to_sock(tcp_sk),
                            TCP_FINWAIT2_TIMEOUT);
                    tcp_sock_release(tcp_sk);
                }
            }
        }
	}
}
