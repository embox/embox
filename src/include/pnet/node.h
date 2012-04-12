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
extern net_node_t pnet_node_init(net_node_t node, net_addr_t addr, pnet_proto_t proto);

/**
 * Free memory allocated by node with its protocol. If protocol is NULL
 * will be called objfree()
 */
extern int pnet_node_free(net_node_t node);

/**
 * There are some nodes (e.g. "devs resolver") that are not in any graph always,
 * but they uses to determine packet behavior (e.g. attach to device for "devs resolver").
 * That means such node is 'supporter'. In other words, supporters are nodes before
 * all graphs.
 */
extern int node_is_supporter(net_node_t node);


#endif /* PNET_NODE_H_ */
