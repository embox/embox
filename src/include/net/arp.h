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

#ifndef NET_ARP_H_
#define NET_ARP_H_

#include <net/netdevice.h>
#include <net/skbuff.h>
#include <arpa/inet.h>

struct sk_buff;
struct net_device;
struct packet_type;

/**
 * Handle arp packet. This function called protocal stack
 * when arp packet has been received
 */
extern int arp_rcv(struct sk_buff *skb, struct net_device *dev,
			struct packet_type *pt, struct net_device *orig_dev);

/**
 * Resolve ip address and rebuild sk_buff
 */
extern int arp_resolve(struct sk_buff *skb);

/**
 * Create and send an ARP packet
 */
extern int arp_send(int type, int ptype, struct net_device *dev,
		in_addr_t dest_ip, in_addr_t src_ip,
		const unsigned char *dest_hw,
		const unsigned char *src_hw, const unsigned char *th);

#endif /* NET_ARP_H_ */
