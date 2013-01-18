/**
 * @file
 * @brief Definitions for the TCP module.
 *
 * @date 03.04.10
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_TCP_H_
#define NET_TCP_H_

#include <hal/arch.h> /* endianess */
#include <types.h>
#include <netinet/tcp.h>

#include <net/inet_sock.h>

#include <framework/mod/options.h>
#include <module/embox/net/tcp_sock.h>


#define MODOPS_AMOUNT_TCP_SOCK OPTION_MODULE_GET(embox__net__tcp_sock, NUMBER, amount_tcp_sock)

typedef struct tcphdr {
	__be16 source;
	__be16 dest;
	__be32 seq;
	__be32 ack_seq;
#if defined(__LITTLE_ENDIAN)
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
#elif defined(__BIG_ENDIAN)
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

#define TCP_V4_HEADER_MIN_SIZE  20
#define TCP_V4_HEADER_SIZE(hdr) (4 * ((struct tcphdr *)hdr)->doff)

enum {
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
	__u32 seq;
	__u16 wind;
};

typedef struct tcp_sock {
	/* inet_sock has to be the first member */
	struct inet_sock inet;      /* Inet socket (parent) */
	__u32 this_unack;           /* Last unacknowledged sequence number */
	struct tcp_seq_state self;  /* Some informations about this socket */
	struct tcp_seq_state rem;   /* Informations about remote socket */
	__u8 lock;                  /* Tool for synchronization */
	struct list_head conn_wait; /* Queue of incoming connection */
	struct event new_conn;      /* Event for new connection notification */
	__u32 seq_queue;            /* Sequence number for next package */
	__u32 ack_flag;             /* Acknowledgment for flags (SYN or FIN) */
	useconds_t last_activity;   /* The time when last message was sent */
	useconds_t oper_timeout;    /* Time out for socket functions */
} tcp_sock_t;

#if 0
enum {
	TCP_OPT_KIND_EOL,
	TCP_OPT_KIND_NOP,
	TCP_OPT_KIND_MSS,
};
#endif

/* Delays in milliseconds */
#define TCP_TIMER_FREQUENCY   1000 /* Frequency for tcp_tmr_default */
#define TCP_TIMEWAIT_DELAY    2000 /* Delay for TIME-WAIT state */
#define TCP_REXMIT_DELAY      2000 /* Delay between rexmitting */
#define TCP_OPER_TIMEOUT      5000 /* Time-out */

#define TCP_WINDOW_DEFAULT    500  /* Default size of widnow */
/* TODO TCP_MAX_DATA_LEN is temporary thing, becouse skb_alloc return NULL
 * if package the big, but it should return skb_queue instead
 */
#define TCP_MAX_DATA_LEN      (ETH_DATA_LEN\
		- (IP_MIN_HEADER_SIZE + TCP_V4_HEADER_MIN_SIZE))  /* Maximum size of data */
/* TCP xmit options */
#define TCP_XMIT_DEFAULT      0    /* Default options for xmitting */
#define TCP_XMIT_IGNORE_DELAY 1    /* Send ignoring delay (checking by default) */
/* Synchronization flags */
#define TCP_SYNC_WRITE_QUEUE  1    /* Synchronization flag for socket sk_write_queue */
#define TCP_SYNC_STATE        2    /* Synchronization flag for socket sk_state */
#define TCP_SYNC_CONN_QUEUE   4    /* Synchronization flag for socket conn_wait */



/* Status of TCP connection */
enum {
	TCP_ST_NOTEXIST, /* Connection does not exist */
	TCP_ST_NONSYNC,  /* Connection is in any non-synchronized state */
	TCP_ST_SYNC      /* Connection is in a synchronized state */
};

/* Union for conversions between socket types */
union sock_pointer {
	struct sock *sk;
	struct inet_sock *inet_sk;
	struct tcp_sock *tcp_sk;
};

static inline struct tcphdr * tcp_hdr(const struct sk_buff *skb) {
	return (struct tcphdr *)skb->h.raw;
}

extern void * get_tcp_sockets(void);
extern const struct proto tcp_prot;
extern struct tcp_sock *tcp_table[MODOPS_AMOUNT_TCP_SOCK];

/* Others functionality */
extern void tcp_free_sock(union sock_pointer sock);
extern void tcp_set_st(union sock_pointer sock, unsigned char new_state);
extern void tcp_obj_lock(union sock_pointer sock, int obj);
extern void tcp_obj_unlock(union sock_pointer sock, int obj);
extern struct sk_buff * alloc_prep_skb(size_t ops_len, size_t data_len);
extern size_t tcp_seq_len(struct sk_buff *skb);
extern size_t tcp_data_left(struct sk_buff *skb);
extern void send_from_sock(union sock_pointer sock, struct sk_buff *skb_send, int xmit_mod);
extern int tcp_st_status(union sock_pointer sock);
extern void debug_print(__u8 code, const char *msg, ...);
extern useconds_t tcp_get_usec(void);

#endif /* NET_TCP_H_ */
