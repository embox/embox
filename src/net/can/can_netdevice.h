/**
 * @file
 *
 * @date 23.04.2012
 * @author Anton Bondarev
 */

#ifndef __CAN_NETDEVICE_H_
#define __CAN_NETDEVICE_H_

extern int cannetdev_register(struct net_device *netdev);

extern struct net_device *cannetdev_get(const char *name, int idx);

#endif /* __CAN_NETDEVICE_H_ */
