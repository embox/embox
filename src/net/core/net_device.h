/**
 * \file net_device.h
 *
 * \date Mar 4, 2009
 * \author anton
 */

#ifndef NET_DEVICE_H_
#define NET_DEVICE_H_

#include "arp.h"
#include "net/if_ether.h"
#include "ip_v4.h"
#include "icmp.h"
#include "udp.h"
#include "net/net_packet.h"

#define NET_DEVICES_QUANTITY     0x4
#define IFNAMSIZ                 6

typedef struct _net_device_stats {
	unsigned long rx_packets;  /* total packets received       */
	unsigned long tx_packets;  /* total packets transmitted    */
        unsigned long rx_bytes;    /* total bytes received         */
        unsigned long tx_bytes;    /* total bytes transmitted      */
	unsigned long rx_err;      /* bad packets received         */
	unsigned long tx_err;      /* packet transmit problems     */
        unsigned long rx_dropped;  /* no space in pool             */
        unsigned long tx_dropped;  /* no space available in pool   */
        unsigned long multicast;   /* multicast packets received   */
}net_device_stats;

typedef struct _net_device {
	char          name[IFNAMSIZ];          /**< It is the name the interface.*/
	unsigned char hw_addr[ETH_ALEN];       /**< hw address                   */
	unsigned char broadcast[ETH_ALEN];     /**< hw bcast address             */
	unsigned long state;
	unsigned char type;                    /**< interface hardware type      */
	unsigned char addr_len;                /**< hardware address length      */
	unsigned char flags;                   /**< interface flags (a la BSD)   */

	int (*open)(struct _net_device *dev);
	int (*stop)(struct _net_device *dev);
	int (*hard_start_xmit) (net_packet *pack, struct _net_device *dev);
	int (*hard_header) (net_packet *pack, struct _net_device *dev, unsigned short type, void *daddr, void *saddr, unsigned tot_len);
	int (*rebuild_header)(net_packet * pack);
	net_device_stats *(*get_stats)(struct _net_device *dev);
	int (*set_mac_address)(struct _net_device *dev, void *addr);
}net_device;

/**
 * Find an network device by its name
 * @param name name to find
 * @return NULL is returned if no matching device is found.
 */
net_device *netdev_get_by_name(const char *name);

/**
 * Allocate network device
 */
net_device *alloc_netdev();

/**
 * Free network device
 * @param dev net_device handler
 */
void free_netdev(net_device *dev);

#endif /* NET_DEVICE_H_ */
