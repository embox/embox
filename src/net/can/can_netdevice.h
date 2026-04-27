/**
 * @file
 *
 * @date 23.04.2012
 * @author Anton Bondarev
 */

#ifndef __CAN_NETDEVICE_H_
#define __CAN_NETDEVICE_H_

#include <net/netdevice.h>

extern int cannetdev_register(struct net_device *netdev);

extern struct net_device *cannetdev_get(const char *name, int idx);

extern int canif_rx(void *data);

#endif /* __CAN_NETDEVICE_H_ */
