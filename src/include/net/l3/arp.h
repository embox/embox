/**
 * @file
 * @brief Definitions for the ARP protocol.
 * @details RFC 826
 *
 * @date 11.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_L3_ARP_H_
#define NET_L3_ARP_H_

#include <net/netdevice.h>

/**
 * Prototypes
 */
struct net_device;

/**
 * Create and send an ARP packet
 */
extern int arp_send(unsigned short oper, unsigned short paddr_space,
		unsigned char haddr_len, unsigned char paddr_len,
		const void *src_haddr, const void *src_paddr,
		const void *dst_haddr, const void *dst_paddr,
		const void *target_haddr, struct net_device *dev);

#endif /* NET_L3_ARP_H_ */
