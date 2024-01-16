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

#include <endian.h>
#include <sys/time.h>
#include <netinet/tcp.h>


#include <linux/types.h>
#include <linux/list.h>
#include <net/socket/inet_sock.h>
#include <net/socket/inet6_sock.h>

typedef struct tcphdr {
	__be16 source;
	__be16 dest;
	__be32 seq;
	__be32 ack_seq;
#if  __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t res1:4,
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
	uint16_t doff:4,
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
	uint32_t options[]; /* 32bit aligned options */
} __attribute__((packed)) tcphdr_t;

#define TCP_MIN_HEADER_SIZE  sizeof(struct tcphdr)
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

struct tcp_wind {
	uint16_t value;
	uint8_t factor;
	uint32_t size;
};

struct tcp_seq_state {
	uint32_t seq;
	struct tcp_wind wind;
};

struct tcp_listen_info{
    int is_listening; /* 0/1 socket is listening */
    unsigned int backlog; /* backlog option from listen, basically max number of connections*/
    unsigned int wait_queue_len; /* how much more connections can handle*/
    struct list_head conn_wait; /* Queue of incoming waiting connections */
    struct list_head conn_free; /* Queue of free sockets for incoming connections */
};

struct tcp_sock {
	struct proto_sock p_sk;     /* Base proto_sock class (MUST BE FIRST) */
	enum tcp_sock_state state;  /* Socket state */
	struct tcp_seq_state self;  /* Some informations about this socket */
	struct tcp_seq_state rem;   /* Informations about remote socket */
	uint32_t last_ack;          /* Last acknowledged sequence number */
	uint32_t ack_flag;          /* Acknowledgment for flags (SYN or FIN) */
	struct tcp_sock *parent;    /* Listening socket to which it belongs */
	union {
		struct list_head conn_lnk;   /* Link for conn_xxx lists */
		struct list_head conn_ready; /* Queue of incoming ready connections */
	};
    struct tcp_listen_info listening;  /* Information for a listening socket*/
    unsigned int accepted; /*when a child socket becomes active on accept()*/
	unsigned int lock;          /* Tool for synchronization */
	struct timeval syn_time;    /* The time when synchronization started */
	struct timeval ack_time;    /* The time when message was ACKed */
	struct timeval rcv_time;    /* The time when last message was received (ONLY FOR TCP_TIMEWAIT) */
	unsigned int dup_ack;       /* Amount of duplicated packets */
	unsigned int rexmit_mode;   /* Socket in rexmit mode */
};

static inline struct tcp_sock * to_tcp_sock( const struct sock *sk) {
	return (struct tcp_sock *)sk->p_sk;
}

enum {
	TCP_OPT_KIND_NOP  = 1, /* No-Operation */
	TCP_OPT_KIND_MSS  = 2, /* Maximum segment size */
	TCP_OPT_KIND_WS   = 3, /* Window scale */
	TCP_OPT_KIND_SACK = 4, /* SACK Permission */
	TCP_OPT_KIND_TS   = 8  /* Timestamp */
};

/* Delays in milliseconds */
#define TCP_TIMER_FREQUENCY   1000  /* Frequency for tcp_tmr_default */
#define TCP_TIMEWAIT_DELAY    2000  /* Delay for TIME-WAIT state */
#define TCP_REXMIT_DELAY      2000  /* Delay between rexmitting */
#define TCP_SYNC_TIMEOUT      5000  /* Synchronization timeout */

#define TCP_REXMIT_DUP_ACK       5  /* Rexmit after n duplicate ack */

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
extern void tcp_sock_release(struct tcp_sock *tcp_sk);
extern void tcp_sock_set_state(struct tcp_sock *tcp_sk,
		enum tcp_sock_state new_state);
extern void tcp_seq_state_set_wind_value(struct tcp_seq_state *tcp_seq_st,
		uint16_t value);
extern void tcp_seq_state_set_wind_factor(struct tcp_seq_state *tcp_seq_st,
		uint8_t factor);
extern void tcp_sock_lock(struct tcp_sock *sk, unsigned int obj);
extern void tcp_sock_unlock(struct tcp_sock *sk, unsigned int obj);
extern int alloc_prep_skb(struct tcp_sock *tcp_sk, size_t opt_len,
		size_t *data_len, struct sk_buff **out_skb);
extern void send_seq_from_sock(struct tcp_sock *tcp_sk, struct sk_buff *skb);
extern int tcp_sock_get_status(struct tcp_sock *tcp_sk);

#endif /* NET_L4_TCP_H_ */
