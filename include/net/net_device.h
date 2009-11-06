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
#define MAX_IFNAME_LENGTH        0x6

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

struct sk_buff;

/**
 * Network device statistics.
 */
typedef struct _net_device_stats {
    unsigned long rx_packets;             /* total packets received       */
    unsigned long tx_packets;             /* total packets transmitted    */
    unsigned long rx_bytes;               /* total bytes received         */
    unsigned long tx_bytes;               /* total bytes transmitted      */
    unsigned long rx_err;                 /* bad packets received         */
    unsigned long tx_err;                 /* packet transmit problems     */
    unsigned long rx_dropped;             /* no space in pool             */
    unsigned long tx_dropped;             /* no space available in pool   */
    unsigned long multicast;              /* multicast packets received   */
    unsigned long collisions;

    /* detailed rx_errors: */
    unsigned long   rx_length_errors;
    unsigned long   rx_over_errors;       /* receiver ring buff overflow  */
    unsigned long   rx_crc_errors;        /* recved pkt with crc error    */
    unsigned long   rx_frame_errors;      /* recv'd frame alignment error */
    unsigned long   rx_fifo_errors;       /* recv'r fifo overrun          */
    unsigned long   rx_missed_errors;     /* receiver missed packet       */

    /* detailed tx_errors */
    unsigned long   tx_aborted_errors;
    unsigned long   tx_carrier_errors;
    unsigned long   tx_fifo_errors;
    unsigned long   tx_heartbeat_errors;
    unsigned long   tx_window_errors;
}net_device_stats;

typedef struct net_device {
	char          name[MAX_IFNAME_LENGTH];          /**< It is the name the interface.*/
	unsigned char hw_addr[ETH_ALEN];       /**< hw address                   */
	unsigned char broadcast[ETH_ALEN];     /**< hw bcast address             */
	unsigned long state;
	unsigned char type;                    /**< interface hardware type      */
	unsigned char addr_len;                /**< hardware address length      */
	unsigned int  flags;                   /**< interface flags (a la BSD)   */
	unsigned      mtu;                     /**< interface MTU value          */
	unsigned long tx_queue_len;            /**< Max frames per queue allowed */
	unsigned long base_addr;               /**< device I/O address           */
    unsigned int  irq;                     /**< device IRQ number            */

	int (*open)(struct net_device *dev);
	int (*stop)(struct net_device *dev);
	int (*hard_start_xmit) (struct sk_buff *pack, struct net_device *dev);
	int (*hard_header) (struct sk_buff *pack, struct net_device *dev, unsigned short type, void *daddr, void *saddr, unsigned tot_len);
	int (*rebuild_header)(struct sk_buff *pack);
	net_device_stats *(*get_stats)(struct net_device *dev);
	int (*set_mac_address)(struct net_device *dev, void *addr);
}net_device_type;

/**
 * Find an network device by its name
 * @param name name to find
 * @return NULL is returned if no matching device is found.
 */
extern net_device_type *netdev_get_by_name(const char *name);

/**
 * Allocate network device
 */
extern net_device_type *alloc_netdev();

/**
 * Free network device
 * @param dev net_device handler
 */
extern void free_netdev(net_device_type *dev);

/**
 * Get RX/TX stats
 */
extern net_device_stats *get_eth_stat(net_device_type *dev);

#endif /* NET_DEVICE_H_ */
