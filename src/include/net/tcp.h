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
} __attribute__((packed)) tcphdr_t;

#define TCP_V4_HEADER_MIN_SIZE  5
#define TCP_V4_HEADER_SIZE(hdr) ((((struct tcphdr *) hdr)->doff) * 4)
enum {
	TCP_ESTABIL = 1,
	TCP_SYN_SENT,
	TCP_SYN_RECV,
	TCP_LISTEN,
	TCP_CLOSE,
	TCP_MAX_STATE
};

typedef struct tcp_sock {
	/* inet_sock has to be the first member */
	struct inet_sock inet;
	char state;
	__be32 seq;
	__be32 seq_unack;
	__be32 ack_seq;
} tcp_sock_t;

static inline tcphdr_t *tcp_hdr(const sk_buff_t *skb) {
	return (tcphdr_t *) skb->h.raw;
}

#define TCP_INET_SOCK(tcp_sk) ((struct inet_sock *) tcp_sk)
#define TCP_SOCK(tcp_sk) ((struct sock *) tcp_sk)

#endif /* TCP_H_ */
