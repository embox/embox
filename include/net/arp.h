/**
 * @file arp.h
 *
 * @brief Definitions for the ARP protocol.
 * @date Mar 11, 2009
 * @author Anton Bondarev
 */
#ifndef ARP_H_
#define ARP_H_

#include "net/if_arp.h"
#include "types.h"

//todo bad style
struct sk_buff;

/**
 * struct for arp_table_records
 */
typedef struct _ARP_ENTITY {
    unsigned char hw_addr[ETH_ALEN];                   /**< hardware addr */
    in_addr_t     pw_addr;                             /**< protocol addr */
    void          *if_handler;                         /**< net_device */
    unsigned char is_busy;                             /**< internal flag that this entry is used */
}ARP_ENTITY;

#define ARP_CACHE_SIZE         0x100                   /** arp table capacity */

extern ARP_ENTITY arp_table[ARP_CACHE_SIZE];           /** arp table */

/**
 * Functions provided by arp.c
 */

/**
 * resolve ip address and rebuild net_packet
 * @param pack pointer to net_packet struct
 * @param dst_addr IP address
 * @return pointer to net_packet struct if success else NULL *
 */
extern struct sk_buff *arp_resolve_addr(struct sk_buff * pack, in_addr_t dst_addr);

/**
 * Handle arp packet. This function called protocal stack when arp packet has been received
 * @param pack net_packet
 */
extern int arp_received_packet(struct sk_buff *pack);

/**
 * this function add entry in arp table if can
 * @param ifdev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
extern int arp_add_entity(void *ifdev, in_addr_t ipaddr, unsigned char macaddr[6]);

/**
 * this function delete entry from arp table if can
 * @param ifdev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
extern int arp_delete_entity(void *ifdev, in_addr_t ipaddr, unsigned char macaddr[6]);

#endif /* ARP_H_ */
