/**
 * @file
 * @brief Definitions for the IPv6 protocol.
 *
 * @date 16.05.13
 * @author Ilia Vaprol
 */

#ifndef NET_IP6_H_
#define NET_IP6_H_

#include <arpa/inet.h>
#include <netinet/in.h>

#include <net/skbuff.h>
#include <net/inet_sock.h>
#include <hal/arch.h> /* endianess */
#include <net/checksum.h>
#include <net/netdevice.h>
#include <net/sock.h>
#include <linux/types.h>

/**
 * Prototypes
 */
struct sk_buff;
struct packet_type;
struct net_device;
struct sock;

typedef struct ip6hdr {
#if defined(__LITTLE_ENDIAN)
	__u8 priority:4, /* priority */
	version: 4; /* version = 6 */
#elif defined (__BIG_ENDIAN)
	__u8 version:4, /* version = 6 */
		priority:4; /* priority */
#endif
	__u8 flow_lbl;
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

#endif /* NET_IP6_H_ */
