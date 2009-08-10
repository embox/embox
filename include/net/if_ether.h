/**
 * \file if_ether.h
 *
 * \date Mar 11, 2009
 * \author anton
 * \brief Global definitions for the Ethernet IEEE 802.3 interface.
 */

#ifndef IF_ETHER_H_
#define IF_ETHER_H_

#define ETH_ALEN        6                        /* Octets in one ethernet addr */

/**
 * These are the defined Ethernet Protocol ID's.
 */
#define ETH_P_LOOP      (unsigned short)0x0060   /* Ethernet Loopback packet  */
#define ETH_P_IP        (unsigned short)0x0800   /* Internet Protocol packet  */
#define ETH_P_ARP       (unsigned short)0x0806   /* Address Resolution packet */

/**
 * This is an Ethernet frame header.
 */
typedef struct _ethhdr {
	unsigned char dst_addr[ETH_ALEN];       /* destination eth addr */
	unsigned char src_addr[ETH_ALEN];       /* source ether addr    */
	unsigned short type;                    /* packet type ID field */
} __attribute__((packed)) ethhdr;

#define ETH_HEADER_SIZE (sizeof(ethhdr))

/*static inline ethhdr *eth_hdr(struct _net_packet *pack) {
        return (ethhdr*)pack->mac.raw;
}*/

#endif /* IF_ETHER_H_ */
