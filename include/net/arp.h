/**
 * @file arp.h
 *
 * @brief Definitions for the ARP protocol.
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#ifndef ARP_H_
#define ARP_H_

#include <net/if_arp.h>
#include <types.h>
#include <net/skbuff.h>

/**
 * struct for arp_table_records
 */
typedef struct _ARP_ENTITY {
    unsigned char hw_addr[ETH_ALEN];                   /**< hardware addr */
    in_addr_t     pw_addr;                             /**< protocol addr */
    void          *if_handler;                         /**< net_device */
    unsigned char is_busy;                             /**< internal flag that this entry is used */
} ARP_ENTITY;

#define ARP_CACHE_SIZE         0x100                   /** arp table capacity */

extern ARP_ENTITY arp_table[ARP_CACHE_SIZE];           /** arp table */

/**
 * Functions provided by arp.c
 */

extern void arp_init();

/**
 * Handle arp packet. This function called protocal stack
 * when arp packet has been received
 * @param pack net_packet
 */
static int arp_rcv(sk_buff_t *pack, net_device_t *dev,
                      packet_type_t *pt, net_device_t *orig_dev);

/**
 * resolve ip address and rebuild net_packet
 * @param pack pointer to net_packet struct
 * @param dst_addr IP address
 * @return pointer to net_packet struct if success else NULL *
 */
extern sk_buff_t *arp_find(struct sk_buff * pack, in_addr_t dst_addr);

/**
 * Create an arp packet.
 */
extern sk_buff_t* arp_create(void *in_dev, in_addr_t dst_addr);

/**
 * this function add entry in arp table if can
 * @param in_dev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
extern int arp_add_entity(void *in_dev, in_addr_t ipaddr, unsigned char macaddr[6]);

/**
 * this function delete entry from arp table if can
 * @param in_dev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
extern int arp_delete_entity(void *in_dev, in_addr_t ipaddr, unsigned char macaddr[6]);

/**
 * Send an arp packet.
 */
extern void arp_xmit(struct sk_buff *skb);

#endif /* ARP_H_ */
