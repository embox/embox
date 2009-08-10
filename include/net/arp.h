/**
 * \file arp.h
 * \date Mar 11, 2009
 * \author anton
 * \brief Definitions for the ARP protocol.
 */
#ifndef ARP_H_
#define ARP_H_

#include "net/if_arp.h"

struct _net_packet;

/**
 * Functions provided by arp.c
 */

/**
 * resolve ip address and rebuild net_packet
 * @param pack net_packet
 * @param dst_addr IP address
 */
extern struct _net_packet *arp_resolve_addr(struct _net_packet * pack, unsigned char dst_addr[4]);

/**
 * Handle arp packet
 * @param pack net_packet
 */
extern int arp_received_packet(struct _net_packet *pack);

/**
 * Set up a new ARP table entry
 * @param ipaddr IP address
 * @param macaddr MAC address
 */
extern int arp_add_entity(void *ifdev, unsigned char ipaddr[4], unsigned char macaddr[6]);

/**
 * Delete a ARP table entry
 * @param ipaddr IP address
 * @param macaddr MAC address
 */
extern int arp_delete_entity(void *ifdev, unsigned char ipaddr[4], unsigned char macaddr[6]);

/**
 * Print ARP table
 */
extern int print_arp_cache(void *ifdev);

#endif /* ARP_H_ */
