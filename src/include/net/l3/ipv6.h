/**
 * @file
 * @brief Definitions for the IPv6 protocol.
 *
 * @date 16.05.13
 * @author Ilia Vaprol
 */

#ifndef NET_L3_IP6_H_
#define NET_L3_IP6_H_

#include <netinet/in.h>
#include <net/skbuff.h>
#include <endian.h>
#include <linux/types.h>
#include <endian.h>
#include <embox/net/pack.h>

/**
 * Prototypes
 */
struct sk_buff;

typedef struct ip6hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	__u8 priority:4, /* priority */
	version: 4; /* version = 6 */
#elif __BYTE_ORDER == __BIG_ENDIAN
	__u8 version:4,
		priority:4;
#endif
	__u8 flow_lbl[3];
	__be16 payload_len;
	__u8 nexthdr;
	__u8 hop_limit;
	struct in6_addr saddr;
	struct in6_addr daddr;
} __attribute__((packed)) ip6hdr_t;

#define IP6_HEADER_SIZE   (sizeof(struct ip6hdr))

static inline ip6hdr_t *ip6_hdr(const struct sk_buff *skb) {
	return skb->nh.ip6h;
}

/**
 * IPV6 packet outgoing options
 */
extern const struct net_pack_out_ops *const ip6_out_ops
		__attribute__ ((weak));

#endif /* NET_L3_IP6_H_ */
