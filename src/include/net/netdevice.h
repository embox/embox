/**
 * @file
 * @brief Definitions for the Interfaces handler.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 */

#ifndef NET_NETDEVICE_H_
#define NET_NETDEVICE_H_

//#include <util/array.h>
#include <net/if.h>
//#include <arpa/inet.h>

#include <net/skbuff.h>
#include <util/list.h>

//#include <util/hashtable.h>

/**
 * Prototypes
 */
struct net_node;
struct net_device;
struct sk_buff;

/* Backlog congestion levels */
#define NET_RX_SUCCESS 0
#define NET_RX_DROP    1

/** Largest hardware address length */
#define MAX_ADDR_LEN 16

/**
 * Network device statistics structure.
 */
typedef struct net_device_stats {
	unsigned long rx_packets; /* total packets received */
	unsigned long tx_packets; /* total packets transmitted */
	unsigned long rx_bytes;   /* total bytes received */
	unsigned long tx_bytes;   /* total bytes transmitted */
	unsigned long rx_err;     /* bad packets received */
	unsigned long tx_err;     /* packet transmit problems */
	unsigned long rx_dropped; /* no space in pool */
	unsigned long tx_dropped; /* no space available in pool */
	unsigned long multicast;  /* multicast packets received */
	unsigned long collisions; /* collision errors */

	/* detailed rx_errors: */
	unsigned long rx_length_errors; /* recived packet with incorrect length */
	unsigned long rx_over_errors;   /* receiver ring buff overflow */
	unsigned long rx_crc_errors;    /* recved pkt with crc error */
	unsigned long rx_frame_errors;  /* recv'd frame alignment error */
	unsigned long rx_fifo_errors;   /* recv'r fifo overrun */
	unsigned long rx_missed_errors; /* receiver missed packet */

	/* detailed tx_errors: */
	unsigned long tx_aborted_errors;
	unsigned long tx_carrier_errors;
	unsigned long tx_fifo_errors;
	unsigned long tx_heartbeat_errors;
	unsigned long tx_window_errors;
} net_device_stats_t;

/**
 * This structure defines the management hooks for network devices.
 * The following hooks can be defined; unless noted otherwise, they are
 * optional and can be filled with a null pointer.
 */
typedef struct net_device_ops {
	int (*ndo_open)(struct net_device *dev);
	int (*ndo_stop)(struct net_device *dev);
	int (*ndo_start_xmit)(struct sk_buff *pack, struct net_device *dev);
	int (*ndo_set_mac_address)(struct net_device *dev, const void *addr);
} net_device_ops_t;

/**
 * structure for control header
 */
typedef struct header_ops {
	int (*create)(struct sk_buff *skb, struct net_device *dev,
			unsigned short type, const void *daddr, const void *saddr);
	int (*rebuild)(struct sk_buff *skb);
} header_ops_t;

/**
 * structure of net device
 */
typedef struct net_device {
	struct list_link rx_lnk;
	char name[IFNAMSIZ]; /**< Name of the interface.  */
	unsigned char dev_addr[MAX_ADDR_LEN]; /**< hw address              */
	unsigned char broadcast[MAX_ADDR_LEN]; /**< hw bcast address        */
	unsigned short type; /**< interface hardware type      */
	unsigned char addr_len; /**< hardware address length      */
	unsigned int flags; /**< interface flags (a la BSD)   */
	unsigned int mtu; /**< interface MTU value          */
#if 1 /* TODO unused field */
	unsigned long tx_queue_len; /**< Max frames per queue allowed */
#endif
	unsigned long base_addr; /**< device I/O address           */
	unsigned int irq; /**< device IRQ number            */
	struct net_device_stats stats;
	const struct net_device_ops *netdev_ops; /**< Management operations        */
	const struct header_ops *header_ops; /**< Hardware header description  */
	struct sk_buff_head dev_queue;
	struct sk_buff_head tx_dev_queue;
	struct sk_buff_head txing_queue;
	void (*poll)(struct net_device *dev);
	struct net_node *pnet_node;
} net_device_t;

/**
 * Find an network device by its name
 * @param name name to find
 * @return NULL is returned if no matching device is found.
 */
extern struct net_device * netdev_get_by_name(const char *name);

/**
 * Allocate network device
 * @param name device name format string
 * @param callback to initialize device
 */
extern struct net_device * netdev_alloc(const char *name,
		void (*setup)(struct net_device *));

/**
 * Free network device
 * @param dev net_device handler
 */
extern void netdev_free(struct net_device *dev);

/**
 * Register network device
 * @param dev net_device handler
 */
extern int netdev_register(struct net_device *dev);

/**
 * Unregister network device
 * @param dev net_device handler
 */
extern int netdev_unregister(struct net_device *dev);

/* TODO this use only for pnet */
extern struct hashtable *netdevs_table;
#define netdev_foreach(device)                                                      \
	for (char **dev_name_ptr = hashtable_get_key_first(netdevs_table);              \
			(dev_name_ptr != NULL) && (device = netdev_get_by_name(*dev_name_ptr)); \
			dev_name_ptr = hashtable_get_key_next(netdevs_table, dev_name_ptr))

/**
 * Pepare an interface for use.
 * @param dev device to open
 */
extern int netdev_open(struct net_device *dev);

/**
 * Shutdown an interface.
 * @param dev device to close
 */
extern int netdev_close(struct net_device *dev);

/**
 * Up flag on device.
 * @param dev device to set flags
 * @param flags
 */
extern int netdev_flag_up(struct net_device *dev, unsigned int flag);

/**
 * Down flag on device.
 * @param dev device to set flags
 * @param flags
 */
extern int netdev_flag_down(struct net_device *dev, unsigned int flag);

/**
 * Set MAC address
 * @param dev device
 * @param macaddr - MAC devices address
 */
extern int netdev_set_macaddr(struct net_device *dev,
		const void *mac_addr);

extern int netdev_set_irq(struct net_device *dev, int irq_num);
extern int netdev_set_baseaddr(struct net_device *dev,
		unsigned long base_addr);
extern int netdev_set_txqueuelen(struct net_device *dev,
		unsigned long new_len);
extern int netdev_set_bcastaddr(struct net_device *dev,
		const void *bcast_addr);

/**
 * this function call ip protocol,
 * it call rebuild mac header function,
 * if can resolve dest addr else it send arp packet,
 * and will trying send packet late. After this
 * it send packet by calling dev_xmit_skb() function
 * return 0 if success else -1
 */
extern int dev_send_skb(struct sk_buff *pack);

/**
 * this function call xmit functions of network device
 * if this device is works (i.e. flags has IFF_UP bit)
 * return 0 if success
 */
extern int dev_xmit_skb(struct sk_buff *pack);

/**
 * this function remove `dev` from queue of device
 * that have received packages
 * @param dev - net_device to be processed
 */
extern void netdev_rx_queued(struct net_device *dev);

/**
 * this function processing a device which had received
 * packages
 */
extern void netdev_rx_processing(void);

/**
 * function must call from net drivers when packet was received
 * and need transmit one throw protocol's stack
 * @param net_packet *pack struct of network packet
 * @return on success, returns 0, on error, -1 is returned
 */
extern int netif_rx(void *pack);

/**
 * Called by irq handler.
 */

/**
 * Save sk_buff to queue of incoming packages of corresponding
 * net_device and prepare this device for processing
 */
extern void netif_rx_schedule(struct sk_buff *skb);

/**
 * This funciton starts stack handling of incoming package
 * @param skb - incoming package which should be handled
 */
extern int netif_receive_skb(struct sk_buff *skb);

#endif /* NET_NETDEVICE_H_ */
