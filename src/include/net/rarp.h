/**
 * @file
 * @brief Definitions for the RARP protocol.
 * @details RFC 903
 *
 * @date 29.09.12
 * @author Ilia Vaprol
 */

#ifndef NET_RARP_H_
#define NET_RARP_H_

#include <net/skbuff.h>
#include <net/netdevice.h>

/**
 * Prototypes
 */
struct sk_buff;
struct net_device;
struct packet_type;

/**
 * Handler of RARP packages
 */
extern int rarp_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev);

/**
 * Create and send an arp packet.
 */
extern int rarp_send(unsigned short oper, unsigned short paddr_space,
		unsigned char haddr_len, unsigned char paddr_len,
		const unsigned char *src_haddr, const unsigned char *src_paddr,
		const unsigned char *dst_haddr, const unsigned char *dst_paddr,
		const unsigned char *target_haddr, struct net_device *dev);

#endif /* NET_RARP_H_ */
