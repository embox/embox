/**
 * @file
 * @brief Definitions for the RARP protocol.
 * @details RFC 903
 *
 * @date 29.09.12
 * @author Ilia Vaprol
 */

#ifndef NET_L3_RARP_H_
#define NET_L3_RARP_H_

#include <net/netdevice.h>

/**
 * Prototypes
 */
struct net_device;

/**
 * Create and send an arp packet.
 */
extern int rarp_send(unsigned short oper, unsigned short paddr_space,
		unsigned char haddr_len, unsigned char paddr_len,
		const void *source_haddr, const void *source_paddr,
		const void *dest_haddr, const void *dest_paddr,
		const void *target_haddr, struct net_device *dev);

#endif /* NET_L3_RARP_H_ */
