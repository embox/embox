/**
 * @file
 * @brief Definitions for the TCP module.
 *
 * @date 03.04.10
 * @author Nikolay Korotky
 */

#ifndef TCP_H_
#define TCP_H_

#include <hal/arch.h>

#include <net/inet_sock.h>

typedef struct tcphdr {
	__be16  source;
	__be16  dest;
	__be32  seq;
	__be32  ack_seq;
#if defined(__LITTLE_ENDIAN)
	__u16   res1:4,
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
	__u16   doff:4,
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
	__be16  window;
	uint16_t check;
	__be16  urg_ptr;
	__u8	options;
} __attribute__((packed)) tcphdr_t;

struct tcp_pseudohdr {
	__be32	saddr;
	__be32	daddr;
	__u8	zero;
	__u8	protocol;
	__be16  tcp_len;
} __attribute__((packed));

#define TCP_V4_HEADER_MIN_SIZE  20
#define TCP_V4_HEADER_SIZE(hdr) ((((struct tcphdr *) hdr)->doff) << 2)
enum {
	TCP_NONE_STATE = 0,
	TCP_ESTABIL = 1,
	TCP_ESTABIL_ACK_WAIT,
	TCP_SYN_SENT,
	TCP_SYN_RECV_PRE,
	TCP_SYN_RECV_PRE2,
	TCP_SYN_RECV,
	TCP_LISTEN,
	TCP_CLOSE,		// 8
	TCP_FINWAIT_1,
	TCP_FINWAIT_2,
	TCP_CLOSING,
	TCP_MAX_STATE
};

struct tcp_seq_state {
	unsigned long  seq;
	unsigned short wind;
};

typedef struct tcp_sock {
	/* inet_sock has to be the first member */
	struct inet_sock inet;
#if 0
	__be32 seq;
	__be32 seq_unack;
	__be32 ack_seq;
#endif
	unsigned long this_unack;
	struct tcp_seq_state this;
	struct tcp_seq_state rem;
	unsigned short mss;

	struct sk_buff_head *conn_wait;

	struct list_head rexmit_link;
} tcp_sock_t;

static inline tcphdr_t *tcp_hdr(const sk_buff_t *skb) {
	return (tcphdr_t *) skb->h.raw;
}

#define TCP_INET_SOCK(tcp_sk) ((struct inet_sock *) tcp_sk)
#define TCP_SOCK(tcp_sk) ((struct sock *) tcp_sk)

enum {
	TCP_OPT_KIND_EOL,
	TCP_OPT_KIND_NOP,
	TCP_OPT_KIND_MSS,
};

typedef int (*tcp_handler_t)(struct tcp_sock *tcpsk,
		struct sk_buff *skb, tcphdr_t *tcph, tcphdr_t *out_tcph);

#endif /* TCP_H_ */
