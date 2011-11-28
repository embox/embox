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

enum {
	TCP_ESTABIL = 1,
	TCP_SYN_SENT,
	TCP_SYN_RECV,
	TCP_LISTEN,
	TCP_CLOSE
};

typedef struct tcp_sock {
	/* inet_sock has to be the first member */
	struct inet_sock inet;
	char state;
} tcp_sock_t;

static inline tcphdr_t *tcp_hdr(const sk_buff_t *skb) {
	return (tcphdr_t *) skb->h.raw;
}

#endif /* TCP_H_ */
