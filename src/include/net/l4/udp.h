/**
 * @file
 * @brief Definitions for the UDP module.
 *
 * @date 26.03.09
 * @author Nikolay Korotky
 */

#ifndef NET_L4_UDP_H_
#define NET_L4_UDP_H_

#include <net/socket/inet_sock.h>

#include <framework/mod/options.h>
#include <module/embox/net/udp_sock.h>
#include <linux/types.h>

#define MODOPS_AMOUNT_UDP_SOCK OPTION_MODULE_GET(embox__net__udp_sock, NUMBER, amount_udp_sock)

struct sock;

typedef struct udphdr {
	__be16 source;
	__be16 dest;
	__be16 len;
	__be16 check;
} __attribute__((packed)) udphdr_t;

#define UDP_HEADER_SIZE	(sizeof(udphdr_t))

static inline udphdr_t *udp_hdr(const sk_buff_t *skb) {
	return (udphdr_t *) skb->h.raw;
}

typedef struct udp_sock {
	/* inet_sock has to be the first member */
	struct inet_sock inet;
} udp_sock_t;

static inline struct udp_sock * udp_sk(struct sock *sk) {
	return (struct udp_sock *)sk;
}

extern const struct sock_ops *const udp_sock_ops __attribute__((weak));

#endif /* NET_L4_UDP_H_ */
