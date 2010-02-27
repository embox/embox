/**
 * @file
 * @brief Global definitions for the Ethernet IEEE 802.3 interface.
 *
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#ifndef IF_ETHER_H_
#define IF_ETHER_H_

#include <types.h>
#include <net/skbuff.h>

/**
 * IEEE 802.3 Ethernet magic constants.
 */
#define ETH_ALEN        6               /* Octets in one ethernet addr */
#define ETH_HLEN        14              /* Total octets in header.     */
#define ETH_ZLEN        60              /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN    1500            /* Max. octets in payload        */
#define ETH_FRAME_LEN   1514            /* Max. octets in frame sans FCS */
#define ETH_FCS_LEN     4               /* Octets in the FCS             */

/**
 * These are the defined Ethernet Protocol ID's.
 */
#define ETH_P_LOOP      (unsigned short)0x0060   /* Ethernet Loopback packet  */
#define ETH_P_IP        (unsigned short)0x0800   /* Internet Protocol packet  */
#define ETH_P_ARP       (unsigned short)0x0806   /* Address Resolution packet */
#define ETH_P_ALL       (unsigned short)0x0003   /* Every packet (be careful!!!) */

/**
 * This is an Ethernet frame header.
 */
typedef struct ethhdr {
	unsigned char h_dest[ETH_ALEN];       /**< destination eth addr */
	unsigned char h_source[ETH_ALEN];     /**< source ether addr    */
	__be16        h_proto;                /**< packet type ID field */
} __attribute__((packed)) ethhdr_t;

#define ETH_HEADER_SIZE (sizeof(struct ethhdr))

/**
 * extract MAC header for skb
 * @param skb
 * @return pointer to MAC header
 */
static inline struct ethhdr *eth_hdr(const sk_buff_t *skb) {
	return (struct ethhdr*)skb->mac.raw;
}

#endif /* IF_ETHER_H_ */
