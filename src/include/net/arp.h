/**
 * @file
 * @brief Definitions for the ARP protocol.
 * @details RFC 826
 *
 * @date 11.05.11
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_ARP_H_
#define NET_ARP_H_

#include <stdint.h>
#include <net/if_arp.h>
#include <net/inetdevice.h>

#define ARP_RESOLVE_TIMEOUT 100

/**
 * Handle arp packet. This function called protocal stack
 * when arp packet has been received
 * @param pack net_packet
 */
extern int arp_rcv(sk_buff_t *pack, net_device_t *dev,
			packet_type_t *pt, net_device_t *orig_dev);

/**
 * resolve ip address and rebuild net_packet
 * @param pack pointer to net_packet struct
 * @param haddr MAC address
 * @return pointer to net_packet struct if success else NULL *
 */
extern int arp_resolve(sk_buff_t *pack);

/**
 * Create and send an arp packet.
 */
extern int arp_send(int type, int ptype, in_addr_t dest_ip,
			struct net_device *dev, in_addr_t src_ip,
			const unsigned char *dest_hw,
			const unsigned char *src_hw, const unsigned char *th);

#endif /* NET_ARP_H_ */
