/**
 * @file
 * @brief Definitions for the UDP module.
 *
 * @date 26.03.09
 * @author Nikolay Korotky
 */

#ifndef NET_L4_UDP_H_
#define NET_L4_UDP_H_

#include <linux/types.h>
#include <net/skbuff.h>

typedef struct udphdr {
	__be16 source;
	__be16 dest;
	__be16 len;
	__be16 check;
} __attribute__((packed)) udphdr_t;

#define UDP_HEADER_SIZE	(sizeof(udphdr_t))

static inline struct udphdr * udp_hdr(const struct sk_buff *skb) {
	return skb->h.uh;
}

extern const struct sock_proto_ops *const udp_sock_ops __attribute__((weak));

#endif /* NET_L4_UDP_H_ */
