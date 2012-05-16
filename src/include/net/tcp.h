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

#include <hal/arch.h>
#include <net/inet_sock.h>
#include <time.h>
#include <types.h>

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
	struct tcp_seq_state this;  /* Some informations about this socket */
	struct tcp_seq_state rem;   /* Informations about remote socket */
	__u8 lock;                  /* Tool for synchronization */
	struct list_head conn_wait; /* Queue of incoming connection */
	__u32 seq_queue;            /* Sequence number for next package */
	__u32 ack_flag;             /* Acknowledgment for flags (SYN or FIN) */
	clock_t last_activity;      /* The time when last message was sent */
} tcp_sock_t;

#if 0
enum {
	TCP_OPT_KIND_EOL,
	TCP_OPT_KIND_NOP,
	TCP_OPT_KIND_MSS,
};
#endif


static inline struct tcphdr * tcp_hdr(const struct sk_buff *skb) {
	return (struct tcphdr *)skb->h.raw;
}

extern void * get_tcp_sockets(void);

#endif /* NET_TCP_H_ */
