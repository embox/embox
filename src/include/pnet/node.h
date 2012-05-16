/**
 * @file
 * @brief
 *
 * @date 25.10.2011
 * @author Anton Kozlov
 */

#ifndef PNET_NODE_H_
#define PNET_NODE_H_

#include <pnet/types.h>

extern net_node_t pnet_node_get(net_node_t node, net_id_t id);
extern int pnet_node_attach(net_node_t node, net_id_t id, net_node_t parent);

/**
 * Allocate memory for node by its protocol. If protocol is NULL
 * will be called objalloc()
 */
extern net_node_t pnet_node_alloc(net_addr_t addr, pnet_proto_t proto);
extern net_node_t pnet_node_init(net_node_t node, pnet_proto_t proto);

/**
 * Free memory allocated by node with its protocol. If protocol is NULL
 * will be called objfree()
 */
extern int pnet_node_free(net_node_t node);

extern net_node_t pnet_get_dev_by_device(struct net_device *dev);
extern net_node_t pnet_get_dev_by_name(char *name);

#endif /* PNET_NODE_H_ */
