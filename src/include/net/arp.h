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

/**
 * struct for arp_table_records
 */
typedef struct {
	uint8_t hw_addr[ETH_ALEN]; /* hardware addr */
	in_addr_t pw_addr;         /* protocol addr */
	in_device_t *if_handler;   /* inet device */
	uint32_t ctime;            /* time to alive */
	uint8_t state;             /* valid or not */
	uint8_t flags;             /* ATF_COM or ATF_COM */
} arp_entity_t;

/* ARP Flag values. */
#define ATF_COM         0x02          /* completed entry */
#define ATF_PERM        0x04          /* permanent entry */

/*
 * After that time, an unused entry is deleted from the arp table.
 * RFC1122 recommends set it to 60*HZ, if your site uses proxy arp
 * and dynamic routing.
 */
#define ARP_TIMEOUT          60000

/*
 * How often is ARP cache checked for expire.
 * It is useless to set ARP_CHECK_INTERVAL > ARP_TIMEOUT
 */
#define ARP_CHECK_INTERVAL   10000

/*
 * If an arp request is send, ARP_RES_TIME is the timeout value until the
 * next request is send.
 * RFC1122: Throttles ARPing, as per 2.3.2.1. (MUST)
 * The recommended minimum timeout is 1 second per destination.
 */
#define ARP_RES_TIME         1000

/*
 * Soft limit on ARP cache size.
 * Note that this number should be greater than
 * number of simultaneously opened sockets, or else
 * hardware header cache will not be efficient.
 */
#define ARP_CACHE_SIZE     CONFIG_ARP_CACHE_SIZE

extern arp_entity_t arp_cache[ARP_CACHE_SIZE];  /** arp table */

/**
 * This will find an entry in the ARP table and return hardware address
 */
extern uint8_t * arp_lookup(in_device_t *in_dev, in_addr_t dst_addr);

/**
 * Functions provided by arp.c
 */
//FIXME arp protocol & arp table must be separated
//extern void arp_init(void);

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

/**
 * this function add entry in arp table if can
 * @param in_dev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return 0 if success, otherwise error code
 */
extern int arp_add_entity(in_device_t *in_dev, in_addr_t ipaddr, uint8_t *hw, uint8_t flags);

/**
 * this function delete entry from arp table if can
 * @param in_dev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return 0 if success, otherwise error code
 */
extern int arp_delete_entity(in_device_t *in_dev, in_addr_t ipaddr, unsigned char *hw);

#endif /* NET_ARP_H_ */
