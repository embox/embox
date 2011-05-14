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

#ifndef ARP_H_
#define ARP_H_

#include <net/if_arp.h>
#include <net/inetdevice.h>

/**
 * struct for arp_table_records
 */
typedef struct {
	unsigned char hw_addr[ETH_ALEN];  /**< hardware addr */
	in_addr_t pw_addr;                /**< protocol addr */
	in_device_t *if_handler;          /**< inet device */
	unsigned int ctime;               /**< time to alive */
	unsigned int state;
	unsigned int flags;
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
 * The number of times an broadcast arp request is send, until
 * the host is considered temporarily unreachable.
 */
#define ARP_MAX_TRIES        3 /* 2 */

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

extern arp_entity_t arp_tables[ARP_CACHE_SIZE];  /** arp table */

/**
 * This will find an entry in the ARP table.
 */
extern int arp_lookup(in_device_t *in_dev, in_addr_t dst_addr);

/**
 * Queue an IP packet, while waiting for the ARP reply packet.
 */
extern void arp_queue(sk_buff_t *skb);

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
 * Create an arp packet.
 */
extern sk_buff_t *arp_create(int type, int ptype, in_addr_t dest_ip,
			net_device_t *dev, in_addr_t src_ip,
			const unsigned char *dest_hw, const unsigned char *src_hw,
			const unsigned char *target_hw);

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
 * @return number of entry in table if success else -1
 */
extern int arp_add_entity(in_device_t *in_dev, in_addr_t ipaddr, unsigned char *hw, unsigned int flags);

/**
 * this function delete entry from arp table if can
 * @param in_dev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
extern int arp_delete_entity(in_device_t *in_dev, in_addr_t ipaddr, unsigned char *hw);

#if 0
/**
 * Send an arp packet.
 */
extern int arp_xmit(sk_buff_t *skb);
#endif

#endif /* ARP_H_ */
