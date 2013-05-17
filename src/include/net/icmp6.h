/**
 * @file
 * @brief Definitions for the ICMP protocol.
 *
 * @date 17.05.13
 * @author Ilia Vaprol
 */

#ifndef NET_ICMP6_H_
#define NET_ICMP6_H_

#include <net/skbuff.h>

/* Types */
enum {
	ICMP6_ECHO_REQUEST = 128,
	ICMP6_ECHO_REPLY = 129
};

typedef struct icmp6hdr {
	__u8     type;
	__u8     code;
	__be16 checksum;
	union {
		struct {
			__be16 id;
			__be16 sequence;
		} echo;
	} un;
} __attribute__((packed)) icmp6hdr_t;

#define ICMP6_HEADER_SIZE	(sizeof(struct icmphdr))

static inline icmp6hdr_t *icmp6_hdr(const struct sk_buff *skb) {
	return skb->h.icmp6h;
}

#endif /* NET_ICMP6_H_ */
