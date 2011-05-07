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
#include <net/if.h>
#include <net/in.h>
#include <lib/list.h>

/* Backlog congestion levels */
#define NET_RX_SUCCESS       0
#define NET_RX_DROP          1
#define NET_RX_BAD           2

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
	int (*ndo_start_xmit)(sk_buff_t *pack, struct net_device *dev);
	int (*ndo_set_mac_address)(struct net_device *dev, void *addr);
	net_device_stats_t* (*ndo_get_stats)(struct net_device *dev);
} net_device_ops_t;

typedef struct header_ops {
	int (*rebuild)(sk_buff_t *pack);
	int (*create)(sk_buff_t *pack, struct net_device *dev, unsigned short type,
			void *daddr, void *saddr, unsigned len);
	int (*parse)(const sk_buff_t *pack, unsigned char *haddr);
} header_ops_t;

/**
 * structure for register incoming protocol packets type
 */
typedef struct packet_type {
	__be16 type; /**< This is really htons(ether_type) */
	struct net_device *dev; /**< NULL is wildcarded here	     */
	int (*func)(sk_buff_t *, struct net_device *, struct packet_type *,
			struct net_device *);
#if 0
	struct sk_buff *(*gso_segment)(sk_buff_t *skb,
			int features);
	int (*gso_send_check)(struct sk_buff *skb);
#endif
	void *af_packet_priv;
	struct list_head list;
	int (*init)(void); /**<Function's called during net subsystem loading
	 process */
} packet_type_t;

#define DECLARE_NET_PACKET(net_packet) \
    static const packet_type_t *pdescriptor \
		__attribute__ ((used, section(".ipstack.packets"))) \
		= &net_packet
/**
 * structure of net device
 */
typedef struct net_device {
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
	void *priv; /**< pointer to private data      */
	struct sk_buff_head dev_queue;
	int (*poll)(struct net_device *dev);
} net_device_t;

static inline void *netdev_priv(struct net_device *dev) {
	return dev->priv;
}

int dev_alloc_name(struct net_device *dev, const char *name);

/**
 * Find an network device by its name
 * @param name name to find
 * @return NULL is returned if no matching device is found.
 */
extern net_device_t *netdev_get_by_name(const char *name);

/**
 * Find an network device by its hw addr
 * @param hwaddr addr to find
 * @return NULL is returned if no matching device is found.
 */
extern net_device_t  *dev_getbyhwaddr(unsigned short type, char *hwaddr);

/**
 * Allocate network device
 * @param name device name format string
 * @param callback to initialize device
 */
extern net_device_t *alloc_netdev(int sizeof_priv, const char *name,
		void(*setup)(net_device_t *));

/**
 * Free network device
 * @param dev net_device handler
 */
extern void free_netdev(net_device_t *dev);

int register_netdev(struct net_device *dev);

void unregister_netdev(struct net_device *dev);

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
extern int dev_open(net_device_t *dev);

/**
 * Shutdown an interface.
 * @param dev device to close
 */
extern int dev_close(net_device_t *dev);

/**
 * Get flags from device.
 * @param dev device to get flags
 */
extern unsigned dev_get_flags(const net_device_t *dev);

/**
 * Set the flags on device.
 * @param dev device to set flags
 * @param flags
 */
extern int dev_change_flags(net_device_t *dev, unsigned flags);

/**
 * this function call ip protocol,
 * it call rebuild mac header function,
 * if can resolve dest addr else it send arp packet and drop this packet
 * and send packet by calling ndo_start_xmit() function
 * return 0 if success else -1
 */
extern int dev_queue_xmit(sk_buff_t *pack);

/**
 * function must call from net drivers when packet was received
 * and need transmit one throw protocol's stack
 * @param net_packet *pack struct of network packet
 * @return on success, returns 0, on error, -1 is returned
 */
extern int netif_rx(sk_buff_t *pack);

/**
 * Called by irq handler.
 */
extern void netif_rx_schedule(net_device_t *dev);

extern int netif_receive_skb(sk_buff_t *skb);

static inline int dev_hard_header(sk_buff_t *skb, net_device_t *dev,
		unsigned short type, void *daddr, void *saddr, unsigned len) {
	if (!dev->header_ops || !dev->header_ops->create) {
		return 0;
	}
	return dev->header_ops->create(skb, dev, type, daddr, saddr, len);
}

static inline void netif_start_queue(net_device_t *dev) {
	/*TODO:*/
}

#endif /* NET_DEVICE_H_ */
