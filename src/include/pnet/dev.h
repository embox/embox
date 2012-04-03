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

extern net_node_t pnet_dev_register(struct net_device *dev);

extern struct net_device *pnet_get_net_device(net_node_t node);

#endif /* PNET_DEV_H_ */
