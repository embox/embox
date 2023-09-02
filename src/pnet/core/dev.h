/**
 * @file
 *
 * @brief
 *
 * @date 03.04.2012
 * @author Anton Bondarev
 */

#ifndef PNET_DEV_H_
#define PNET_DEV_H_

#include <net/netdevice.h>

extern struct net_device *pnet_get_net_device(struct net_node *node);

#endif /* PNET_DEV_H_ */
