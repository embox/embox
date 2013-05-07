/**
 * @file
 * @brief Global definitions for the Ethernet IEEE 802.3 interface.
 *
 * @date 11.03.09
 * @author Anton Bondarev
 */

#ifndef IF_ETHER_H_
#define IF_ETHER_H_

#include <linux/types.h>
#include <net/skbuff.h>

/**
 * Prototypes
 */
struct sk_buff;

/**
 * IEEE 802.3 Ethernet magic constants.
 */
#define ETH_ALEN         6 /* Octets in one ethernet addr */
#define ETH_HLEN        14 /* Total octets in header */
#define ETH_ZLEN        60 /* Min octets in frame sans FCS */
#define ETH_DATA_LEN  1500 /* Max octets in payload */
#define ETH_FRAME_LEN 1514 /* Max octets in frame sans FCS */
#define ETH_FCS_LEN      4 /* Octets in the FCS */

/**
 * These are the defined Ethernet Protocol ID's.
 */
#define ETH_P_LOOP      0x0060   /* Ethernet Loopback packet  */
#define ETH_P_IP        0x0800   /* Internet Protocol packet  */
#define ETH_P_ARP       0x0806   /* Address Resolution packet */
#define ETH_P_RARP      0x8035   /* Revrse Address Resolution packet */
#define ETH_P_ALL       0x0003   /* Every packet (be careful!!!) */

/**
 * This is an Ethernet frame header.
 */
typedef struct ethhdr {
	__u8 h_dest[ETH_ALEN];   /* Destination ethernet address */
	__u8 h_source[ETH_ALEN]; /* Source ethernet address */
	__be16 h_proto;          /* Packet type ID */
} __attribute__((packed)) ethhdr_t;

#define ETH_HEADER_SIZE (sizeof(struct ethhdr))

/**
 * extract MAC header for skb
 * @param skb
 * @return pointer to MAC header
 */
static inline struct ethhdr * eth_hdr(const struct sk_buff *skb) {
	return skb->mac.ethh;
}

#endif /* IF_ETHER_H_ */
