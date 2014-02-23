/**
 * @file
 * @brief An Ethernet Address Resolution Protocol
 * @details RFC 826
 *
 * @date 11.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_L3_ARP_H_
#define NET_L3_ARP_H_

#include <assert.h>
#include <net/skbuff.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct net_device;
struct sk_buff;

/**
 * ARP Hardware Space
 */
enum arp_hdr {
	ARP_HRD_ETHERNET =   1, /* Ethernet device */
	ARP_HRD_LOOPBACK = 772  /* Loopback device */
};

/**
 * ARP Operation Codes
 */
enum arp_op {
	ARP_OP_REQUEST = 1, /* ARP Request */
	ARP_OP_REPLY   = 2  /* ARP Reply */
};

/**
 * ARP Body - Additional information of variable length
 */
struct arpbody {
	uint8_t *ar_sha; /* Sender hardware address */
	uint8_t *ar_spa; /* Sender protocol address */
	uint8_t *ar_tha; /* Target hardware address */
	uint8_t *ar_tpa; /* Target protocol address */
} __attribute__((packed));

/**
 * ARP Header Structure
 */
struct arphdr {
	uint16_t ar_hrd;   /* Hardware address space */
	uint16_t ar_pro;   /* Protocol address space */
	uint8_t ar_hln;    /* Length of hardware address */
	uint8_t ar_pln;    /* Length of protocol address */
	uint16_t ar_op;    /* Operation code */
	uint8_t ar_body[]; /* Message body */
} __attribute__((packed));

#define ARP_MIN_HEADER_SIZE (sizeof(struct arphdr))
#define ARP_HEADER_SIZE(arph) \
	ARP_CALC_HEADER_SIZE((arph)->ar_hln, (arph)->ar_pln)
#define ARP_CALC_HEADER_SIZE(hln, pln) \
	(ARP_MIN_HEADER_SIZE + (hln + pln) * 2)

static inline struct arphdr * arp_hdr(const struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->nh.arph != NULL);
	return skb->nh.arph;
}

/**
 * Discover hardware address by protocol address
 */
extern int arp_discover(struct net_device *dev, uint16_t pro,
		uint8_t pln, const void *spa, const void *tpa);

#endif /* NET_L3_ARP_H_ */
