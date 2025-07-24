/**
 * @file
 * @brief Definitions for the Interfaces handler.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 */

#ifndef NET_NETDEVICE_H_
#define NET_NETDEVICE_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <lib/libds/dlist.h>
#include <net/if.h>
#include <net/net_namespace.h>
#include <net/skbuff.h>

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
#define MAX_ADDR_LEN   16

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
typedef struct net_driver {
	int (*start)(struct net_device *dev);
	int (*stop)(struct net_device *dev);
	int (*xmit)(struct net_device *dev, struct sk_buff *skb);
	int (*set_macaddr)(struct net_device *dev, const void *addr);
	int (*mdio_read)(struct net_device *dev, uint8_t reg);
	int (*mdio_write)(struct net_device *dev, uint8_t reg, uint16_t data);
	void (*set_phyid)(struct net_device *dev, uint8_t phyid);
	int (*set_speed)(struct net_device *dev, int speed);
} net_driver_t;

/**
 * information for/of device header
 */
struct net_header_info {
	unsigned short type; /* packet type */
	const void *src_hw;  /* source hw address
							use device addr if null */
	const void *dst_hw;  /* destination hw address
							if null use dst_p for resolving
							if dst_p is null too, use bcast */
	const void *dst_p;   /* destination protocol address
							used for discovering of the
							hw address in case dst_hw is null */
	unsigned char p_len; /* length of dst_p */
};

/**
 * structure for control header
 */
typedef struct net_device_ops {
	int (*build_hdr)(struct sk_buff *skb,
	    const struct net_header_info *hdr_info);
	int (*check_addr)(const void *addr);
	int (*check_mtu)(int mtu);
} net_device_ops_t;

struct sock_family_ops;
struct sock_proto_ops;

/**
 * structure of net device
 */
typedef struct net_device {
	int index;
	char name[IFNAMSIZ];                   /**< Name of the interface.  */
	unsigned char dev_addr[MAX_ADDR_LEN];  /**< hw address              */
	unsigned char broadcast[MAX_ADDR_LEN]; /**< hw bcast address        */
	unsigned short type;                   /**< interface hardware type      */
	unsigned char hdr_len;                 /**< hardware header length      */
	unsigned char addr_len;                /**< hardware address length      */
	unsigned int flags;                    /**< interface flags (a la BSD)   */
	unsigned int mtu;                      /**< interface MTU value          */
	uintptr_t base_addr;                   /**< device I/O address           */
	unsigned int irq;                      /**< device IRQ number            */
	struct net_device_stats stats;
	const struct net_device_ops *ops; /**< Hardware description  */
	const struct net_driver *drv_ops; /**< Management operations        */
	struct dlist_head rx_lnk;         /* for netif_rx list */
	struct dlist_head tx_lnk;         /* for netif_tx list */
	struct sk_buff_head dev_queue;    /* rx skb queue */
	struct sk_buff_head dev_queue_tx; /* tx skb queue */
	struct net_node *pnet_node;
#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	net_namespace_p net_ns;
#endif
	const struct sock_family_ops *nd_net_offload;
	const struct sock_proto_ops *nd_sock_offload;

	void *priv; /**< private data */
} net_device_t;


#define netdev_net_offload(dev)               \
	({                                        \
		const struct net_device *__dev = dev; \
		assert(__dev != NULL);                \
		__dev->nd_net_offload                 \
	})

#define netdev_sock_offload(dev)              \
	({                                        \
		const struct net_device *__dev = dev; \
		assert(__dev != NULL);                \
		__dev->nd_sock_offload                \
	})

/**
 * Get data private data casted to type
 */
#define netdev_priv(dev)                      \
	({                                        \
		const struct net_device *__dev = dev; \
		assert(__dev != NULL);                \
		__dev->priv;                          \
	})

/**
 * Find an network device by its name
 * @param name name to find
 * @return NULL is returned if no matching device is found.
 */
extern struct net_device *netdev_get_by_name(const char *name);

/**
 * Allocate network device
 * @param name device name format string
 * @param callback to initialize device
 */
extern struct net_device *netdev_alloc(const char *name,
    int (*setup)(struct net_device *), size_t priv_size);

extern void dev_net_set(struct net_device *dev, net_namespace_p net_ns);
extern struct net_device *veth_alloc(struct net_device **v1,
    struct net_device **v2);

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
#define netdev_foreach(device)                                         \
	for (char **dev_name_ptr = hashtable_get_key_first(netdevs_table); \
	     (dev_name_ptr != NULL)                                        \
	     && (device = netdev_get_by_name(*dev_name_ptr));              \
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
extern int netdev_set_macaddr(struct net_device *dev, const void *mac_addr);

extern int netdev_set_irq(struct net_device *dev, int irq_num);
extern int netdev_set_baseaddr(struct net_device *dev, unsigned long base_addr);
extern int netdev_set_bcastaddr(struct net_device *dev, const void *bcast_addr);
extern int netdev_set_mtu(struct net_device *dev, int mtu);

extern void netif_carrier_off(struct net_device *dev);
extern void netif_carrier_on(struct net_device *dev);

#endif /* NET_NETDEVICE_H_ */
