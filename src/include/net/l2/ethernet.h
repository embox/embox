/**
 * @file
 * @brief Definitions for the Ethernet handlers.
 * @details IEEE 802.3
 *
 * @date 05.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_L2_ETHERNET_H_
#define NET_L2_ETHERNET_H_

#include <assert.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct net_device;
struct sk_buff;

/**
 * IEEE 802.3 Ethernet magic constants
 */
#define ETH_ALEN         6 /* Octets in one ethernet addr */
#define ETH_HLEN        14 /* Total octets in header */
#define ETH_ZLEN        60 /* Min octets in frame sans FCS */
#define ETH_DATA_LEN  1500 /* Max octets in payload */
#define ETH_FRAME_LEN 1514 /* Max octets in frame sans FCS */
#define ETH_FCS_LEN      4 /* Octets in the FCS */

/**
 * These are the defined Ethernet Protocol ID's
 */
#define ETH_P_LOOP      0x0060   /* Ethernet Loopback packet  */
#define ETH_P_IP        0x0800   /* Internet Protocol packet  */
#define ETH_P_ARP       0x0806   /* Address Resolution packet */
#define ETH_P_RARP      0x8035   /* Revrse Address Resolution packet */
#define ETH_P_IPV6      0x86DD   /* IPv6 over bluebook */
#define ETH_P_ALL       0x0003   /* Every packet (be careful!!!) */

/**
 * Ethernet Header Structure
 */
struct ethhdr {
	uint8_t  h_dest[ETH_ALEN];   /* Destination ethernet address */
	uint8_t  h_source[ETH_ALEN]; /* Source ethernet address */
	uint16_t h_proto;            /* Packet type ID */
} __attribute__((packed));

#define ETH_HEADER_SIZE (sizeof(struct ethhdr))

static inline struct ethhdr * eth_hdr(const struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->mac.ethh != NULL);
	return skb->mac.ethh;
}

/**
 * Ethernet device options
 */
extern const struct net_device_ops ethernet_ops;

/**
 * Allocate Ethernet device
 */
extern struct net_device * etherdev_alloc(size_t priv_size);

extern void etherdev_free(struct net_device *dev);

#endif /* NET_L2_ETHERNET_H_ */
