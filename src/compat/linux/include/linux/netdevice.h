/**
 * @file
 * @brief
 *
 * @date 19.10.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_LINUX_LINUX_NETDEVICE_H_
#define COMPAT_LINUX_LINUX_NETDEVICE_H_

#include <net/if.h>
#include <net/netdevice.h>

/**
 * Test if the device has been brought up
 *
 * @param dev - network device
 * @retval non-zero if running, otherwise zero
 */
static inline int netif_running(const struct net_device *dev) {
	return dev->flags & IFF_RUNNING;
}

#endif /* COMPAT_LINUX_LINUX_NETDEVICE_H_ */
