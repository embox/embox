/**
 * @file
 *
 * @date 23.04.26
 * @author Anton Bondarev
 */

#ifndef NET_CAN_NETDEVICE_H_
#define NET_CAN_NETDEVICE_H_

#include <net/netdevice.h>

extern struct net_device *can_netdev_alloc(void);
extern void can_netdev_free(struct net_device *dev);
extern int can_netdev_register(struct net_device *netdev);
extern struct net_device *can_netdev_get(const char *name, int idx);

#endif /* NET_CAN_NETDEVICE_H_ */
