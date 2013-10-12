/**
 * @file
 * @brief Definitions for the TCP module.
 *
 * @date 03.04.10
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_L4_TCP_H_
#define NET_L4_TCP_H_

#include <hal/arch.h>
#include <linux/types.h>
#include <linux/list.h>

#include <sys/time.h>

#include <netinet/tcp.h>

#include <net/socket/inet_sock.h>

typedef struct tcphdr {
	__be16 source;
	__be16 dest;
	__be32 seq;
	__be32 ack_seq;
#if  __BYTE_ORDER == __LITTLE_ENDIAN
	__u16 res1:4,
		doff:4,
		fin:1,
		syn:1,
		rst:1,
		psh:1,
		ack:1,
		urg:1,
		ece:1,
		cwr:1;
#elif  __BYTE_ORDER == __BIG_ENDIAN
	__u16 doff:4,
		res1:4,
		cwr:1,
		ece:1,
		urg:1,
		ack:1,
		psh:1,
		rst:1,
		syn:1,
		fin:1;
#endif
	__be16 window;
	__be16 check;
	__be16 urg_ptr;
	__u8 options;
} __attribute__((packed)) tcphdr_t;

struct tcp_pseudohdr {
	__be32 saddr;
	__be32 daddr;
	__u8 zero;
	__u8 protocol;
	__be16 tcp_len;
} __attribute__((packed));

#define TCP_MIN_HEADER_SIZE  20
#define TCP_HEADER_SIZE(hdr) ((hdr)->doff * 4)

enum tcp_sock_state {
	TCP_CLOSED,
	TCP_LISTEN,
	TCP_SYN_SENT,
	TCP_SYN_RECV_PRE,
	TCP_SYN_RECV,
	TCP_ESTABIL,
	TCP_FINWAIT_1,
	TCP_FINWAIT_2,
	TCP_CLOSEWAIT,
	TCP_CLOSING,
	TCP_LASTACK,
	TCP_TIMEWAIT,
	TCP_MAX_STATE
};

struct tcp_seq_state {
	uint32_t seq;
	uint16_t wind;
};

typedef struct tcp_sock {
	struct proto_sock p_sk;     /* Base proto_sock class (MUST BE FIRST) */
	enum tcp_sock_state state;  /* Socket state */
	struct tcp_seq_state self;  /* Some informations about this socket */
	struct tcp_seq_state rem;   /* Informations about remote socket */
	uint32_t last_ack;          /* Last acknowledged sequence number */
	uint32_t ack_flag;          /* Acknowledgment for flags (SYN or FIN) */
	struct tcp_sock *parent;    /* Listening socket to which it belongs */
	struct list_head conn_wait; /* Queue of incoming connection */
	unsigned int conn_wait_len; /* Length of queue of incoming connection */
	unsigned int conn_wait_max; /* Max length of queue of incoming connection */
	unsigned int lock;          /* Tool for synchronization */
	struct timeval syn_time;   /* The time when synchronization started */
	struct timeval ack_time;   /* The time when message was ACKed */
	struct timeval rcv_time;   /* The time when last message was received (ONLY FOR TCP_TIMEWAIT) */
} tcp_sock_t;

static inline struct tcp_sock * to_tcp_sock(struct sock *sk) {
	return (struct tcp_sock *)sk->p_sk;
}

#if 0
enum {
	TCP_OPT_KIND_EOL,
	TCP_OPT_KIND_NOP,
	TCP_OPT_KIND_MSS,
};
#endif

/* Delays in milliseconds */
#define TCP_TIMER_FREQUENCY   1000  /* Frequency for tcp_tmr_default */
#define TCP_TIMEWAIT_DELAY    2000  /* Delay for TIME-WAIT state */
#define TCP_REXMIT_DELAY      2000  /* Delay between rexmitting */
#define TCP_SYNC_TIMEOUT      5000  /* Synchronization timeout */

#define TCP_WINDOW_DEFAULT    16384 /* Default size of widnow */

/**
 * TODO TCP_MAX_DATA_LEN is temporary thing, becouse skb_alloc return NULL
 * if package the big, but it should return skb_queue instead
 */
#define TCP_MAX_DATA_LEN      (ETH_DATA_LEN\
		- (IP_MIN_HEADER_SIZE + TCP_MIN_HEADER_SIZE))  /* Maximum size of data */

/* Synchronization flags */
#define TCP_SYNC_WRITE_QUEUE  0x01 /* Synchronization flag for socket sk_write_queue */
#define TCP_SYNC_STATE        0x02 /* Synchronization flag for socket sk_state */
#define TCP_SYNC_CONN_QUEUE   0x04 /* Synchronization flag for socket conn_wait */
#define TCP_SYNC_SOCK_TABLE   0x08 /* Synchronization flag for tcp_table (set on tcp_sock_default) */

/* Status of TCP connection */
enum {
	TCP_ST_NOTEXIST, /* Connection does not exist */
	TCP_ST_NONSYNC,  /* Connection is in any non-synchronized state */
	TCP_ST_SYNC      /* Connection is in a synchronized state */
};

static inline struct tcphdr * tcp_hdr(const struct sk_buff *skb) {
	return skb->h.th;
}

extern const struct sock_proto_ops *const tcp_sock_ops __attribute__((weak));

/* Others functionality */
extern void build_tcp_packet(size_t opt_len, size_t data_len,
		struct tcp_sock *tcp_sk, struct sk_buff *skb);
extern void tcp_free_sock(struct tcp_sock *tcp_sk);
extern void tcp_set_st(struct tcp_sock *tcp_sk,
		enum tcp_sock_state new_state);
extern void tcp_obj_lock(struct tcp_sock *sk, unsigned int obj);
extern void tcp_obj_unlock(struct tcp_sock *sk, unsigned int obj);
extern struct sk_buff * alloc_prep_skb(size_t ops_len, size_t data_len);
extern void send_data_from_sock(struct tcp_sock *tcp_sk, struct sk_buff *skb);
extern int tcp_st_status(struct tcp_sock *tcp_sk);
extern void debug_print(__u8 code, const char *msg, ...);


#endif /* NET_L4_TCP_H_ */
