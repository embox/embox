/**
 * @file
 * @brief Definitions for the Interfaces handler.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 */

#ifndef NET_DEVICE_H_
#define NET_DEVICE_H_

#include <net/if_ether.h>
//#include <util/array.h>
#include <net/if.h>
//#include <arpa/inet.h>

#include <linux/list.h>

//#include <util/hashtable.h>

/**
 * Prototypes
 */
struct net_node;
struct net_device;
struct sk_buff;


/* Backlog congestion levels */
#define NET_RX_SUCCESS       0
#define NET_RX_DROP          1

/* Driver transmit return codes */
#define NETDEV_TX_OK        0
#define NETDEV_TX_BUSY      1

/** Largest hardware address length */
#define MAX_ADDR_LEN    32

/**
 * Network device statistics structure.
 */
typedef struct net_device_stats {
	unsigned long rx_packets; /**< total packets received       */
	unsigned long tx_packets; /**< total packets transmitted    */
	unsigned long rx_bytes; /**< total bytes received         */
	unsigned long tx_bytes; /**< total bytes transmitted      */
	unsigned long rx_err; /**< bad packets received         */
	unsigned long tx_err; /**< packet transmit problems     */
	unsigned long rx_dropped; /**< no space in pool             */
	unsigned long tx_dropped; /**< no space available in pool   */
	unsigned long multicast; /**< multicast packets received   */
	unsigned long collisions;

	/* detailed rx_errors: */
	unsigned long rx_length_errors;
	unsigned long rx_over_errors; /**< receiver ring buff overflow  */
	unsigned long rx_crc_errors; /**< recved pkt with crc error    */
	unsigned long rx_frame_errors; /**< recv'd frame alignment error */
	unsigned long rx_fifo_errors; /**< recv'r fifo overrun          */
	unsigned long rx_missed_errors; /**< receiver missed packet       */

	/* detailed tx_errors */
	unsigned long tx_aborted_errors;
	unsigned long tx_carrier_errors;
	unsigned long tx_fifo_errors;
	unsigned long tx_heartbeat_errors;
	unsigned long tx_window_errors;
} net_device_stats_t;

enum netdev_state_t {
	__LINK_STATE_START
#if 0
	, __LINK_STATE_PRESENT,
	__LINK_STATE_NOCARRIER,
	__LINK_STATE_LINKWATCH_PENDING,
	__LINK_STATE_DORMANT,
#endif
};

/**
 * This structure defines the management hooks for network devices.
 * The following hooks can be defined; unless noted otherwise, they are
 * optional and can be filled with a null pointer.
 */
typedef struct net_device_ops {
	int (*ndo_open)(struct net_device *dev);
	int (*ndo_stop)(struct net_device *dev);
	int (*ndo_start_xmit)(struct sk_buff *pack, struct net_device *dev);
	int (*ndo_set_mac_address)(struct net_device *dev, void *addr);
	net_device_stats_t* (*ndo_get_stats)(struct net_device *dev);
} net_device_ops_t;

/**
 * structure for control header
 */
typedef struct header_ops {
	int (*create)(struct sk_buff *skb, struct net_device *dev,
			unsigned short type, void *daddr, void *saddr, unsigned len);
	int (*rebuild)(struct sk_buff *skb);
	int (*parse)(const struct sk_buff *skb, unsigned char *haddr);
} header_ops_t;

/**
 * structure for register incoming protocol packets type
 */
typedef struct packet_type {
	__be16 type; /**< This is really htons(ether_type) */
	struct net_device *dev; /**< NULL is wildcarded here	     */
	int (*func)(struct sk_buff *, struct net_device *, struct packet_type *,
			struct net_device *);
	void *af_packet_priv;
	struct list_head list;
	int (*init)(void); /**<Function's called during net subsystem loading
	 process */
} packet_type_t;

/**
 * structure of net device
 */
typedef struct net_device {
	struct list_head rx_dev_link;

	char name[IFNAMSIZ]; /**< Name of the interface.  */
	unsigned char dev_addr[MAX_ADDR_LEN]; /**< hw address              */
	unsigned char broadcast[MAX_ADDR_LEN]; /**< hw bcast address        */
	unsigned long state;
	unsigned short type; /**< interface hardware type      */
	unsigned char addr_len; /**< hardware address length      */
	unsigned int flags; /**< interface flags (a la BSD)   */
	unsigned mtu; /**< interface MTU value          */
	unsigned long tx_queue_len; /**< Max frames per queue allowed */
	unsigned long base_addr; /**< device I/O address           */
	unsigned int irq; /**< device IRQ number            */
	net_device_stats_t stats;
	const net_device_ops_t *netdev_ops; /**< Management operations        */
	const header_ops_t *header_ops; /**< Hardware header description  */
	struct sk_buff_head dev_queue;
	struct sk_buff_head tx_dev_queue;
	struct sk_buff_head txing_queue;
	int (*poll)(struct net_device *dev);
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
 * Add packet handler
 * @param pt packet type declaration
 *
 * Add a protocol handler to the networking stack. The passed &packet_type
 * is linked into kernel lists and may not be freed until it has been
 * removed from the kernel lists.
 */
extern void dev_add_pack(packet_type_t *pt);

/**
 * Remove packet handler
 * @param pt packet type declaration
 *
 * Remove a protocol handler that was previously added to the kernel
 * protocol handlers by dev_add_pack(). The passed &packet_type is removed
 * from the kernel lists and can be freed or reused once this function
 * returns.
 */
extern void dev_remove_pack(packet_type_t *pt);

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
 * Get flags from device.
 * @param dev device to get flags
 */
extern unsigned int netdev_get_flags(const struct net_device *dev);

/**
 * Set the flags on device.
 * @param dev device to set flags
 * @param flags
 */
extern int netdev_set_flags(struct net_device *dev, unsigned flags);

/**
 * Set MAC address
 * @param dev device
 * @param macaddr - MAC devices address
 */
extern int netdev_set_macaddr(struct net_device *dev,
		const unsigned char *macaddr);

/**
 * this function call ip protocol,
 * it call rebuild mac header function,
 * if can resolve dest addr else it send arp packet,
 * and will trying send packet late. After this
 * it send packet by calling dev_queue_xmit() function
 * return 0 if success else -1
 */
extern int dev_queue_send(sk_buff_t *pack);

/**
 * this function call xmit functions of network device
 * if this device is works (i.e. flags has IFF_UP bit)
 * return 0 if success
 */
extern int dev_queue_xmit(sk_buff_t *pack);

/**
 * this function add `dev` to list of those that
 * must be processed
 * @param dev - net_device to be processed
 */
extern void netdev_rx_queued(struct net_device *dev);

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

#endif /* NET_DEVICE_H_ */
