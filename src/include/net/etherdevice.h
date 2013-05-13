/**
 * @file
 * @brief Definitions for the Ethernet handlers.
 * @details IEEE 802.3
 *
 * @date 5.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_ETHERDEVICE_H_
#define NET_ETHERDEVICE_H_

#include <net/netdevice.h>
#include <net/skbuff.h>

/**
 * Prototypes
 */
struct header_ops;
struct net_device;

/**
 * Functions provided by eth.c
 */

/**
 * eth_get_header_ops - returns methods for working with ethernet header
 */
extern const struct header_ops * eth_get_header_ops(void);

/**
 * Functions provided by etherdev.c
 */

/**
 * Set new Ethernet hardware address.
 * @param dev network device
 * @param addr socket address
 */
extern int etherdev_set_macaddr(struct net_device *dev, const void *mac_addr);

/**
 * Set new MTU size
 * @param dev network device
 * @param new_mtu new Maximum Transfer Unit
 */
extern int etherdev_set_mtu(struct net_device *dev, int new_mtu);

#if 0
/**
 * Setup Ethernet network device
 * @param dev network device
 * Fill in the fields of the device structure with Ethernet-generic values.
 */
extern void etherdev_setup(struct net_device *dev);
#endif

/**
 * Allocates and sets up an Ethernet device
 */
extern struct net_device * etherdev_alloc(void);

/**
 * Frees an Ethernet device
 */
extern void etherdev_free(struct net_device *dev);

#endif /* NET_ETHERDEVICE_H_ */
