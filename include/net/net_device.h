/**
 * \file net_device.h
 *
 * \date Mar 4, 2009
 * \author anton
 */

#ifndef NET_DEVICE_H_
#define NET_DEVICE_H_

#include "net/if_ether.h"

#define NET_DEVICES_QUANTITY     0x4
#define IFNAMSIZ                 6

/* Standard interface flags (net_device->flags). */
#define IFF_UP          0x1             /* interface is up              */
#define IFF_BROADCAST   0x2             /* broadcast address valid      */
#define IFF_DEBUG       0x4             /* turn on debugging            */
#define IFF_LOOPBACK    0x8             /* is a loopback net            */
#define IFF_POINTOPOINT 0x10            /* interface is has p-p link    */
#define IFF_NOTRAILERS  0x20            /* avoid use of trailers        */
#define IFF_RUNNING     0x40            /* interface RFC2863 OPER_UP    */
#define IFF_NOARP       0x80            /* no ARP protocol              */
#define IFF_PROMISC     0x100           /* receive all packets          */
#define IFF_ALLMULTI    0x200           /* receive all multicast packets*/
#define IFF_MULTICAST   0x1000          /* Supports multicast           */

struct _net_packet;

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
	unsigned int  flags;                   /**< interface flags (a la BSD)   */
	unsigned      mtu;                     /**< interface MTU value          */
	unsigned long tx_queue_len;            /**< Max frames per queue allowed */

	int (*open)(struct _net_device *dev);
	int (*stop)(struct _net_device *dev);
	int (*hard_start_xmit) (struct _net_packet *pack, struct _net_device *dev);
	int (*hard_header) (struct _net_packet *pack, struct _net_device *dev, unsigned short type, void *daddr, void *saddr, unsigned tot_len);
	int (*rebuild_header)(struct _net_packet *pack);
	net_device_stats *(*get_stats)(struct _net_device *dev);
	int (*set_mac_address)(struct _net_device *dev, void *addr);
}net_device;

/**
 * Find an network device by its name
 * @param name name to find
 * @return NULL is returned if no matching device is found.
 */
extern net_device *netdev_get_by_name(const char *name);

/**
 * Allocate network device
 */
extern net_device *alloc_netdev();

/**
 * Free network device
 * @param dev net_device handler
 */
extern void free_netdev(net_device *dev);

#endif /* NET_DEVICE_H_ */
