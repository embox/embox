/**
 * @file
 * @brief Definitions for the Ethernet handlers.
 * @details IEEE 802.3
 *
 * @date 05.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_L2_ETHERNET_H_
#define NET_L2_ETHERNET_H_

#include <net/netdevice.h>

/**
 * Prototypes
 */
struct net_device;

/**
 * Ethernet device options
 */
extern const struct net_device_ops ethernet_ops;

/**
 * Setup Ethernet network device
 * @param dev network device
 * Fill in the fields of the device structure with Ethernet-generic values.
 */
extern int ethernet_setup(struct net_device *dev);

/**
 * Macros to allocate Ethernet device
 * TODO remove this
 */
static inline struct net_device * etherdev_alloc(void) {
	return netdev_alloc("eth%u", &ethernet_setup);
}

#endif /* NET_L2_ETHERNET_H_ */
