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
#include <net/socket.h>

/**
 * Handler of RARP packages
 */
extern int rarp_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev);

/**
 * Create and send an arp packet.
 */
extern int rarp_send(unsigned short oper, in_addr_t dest_ip, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		struct net_device *dev);

#endif /* NET_ARP_H_ */
