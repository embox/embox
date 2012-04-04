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

extern net_node_t pnet_get_node_null(void);

extern net_node_t pnet_get_node_linux_gate(void);

extern net_node_t pnet_get_node_info(void);

extern net_node_t pnet_node_get(net_node_t node, net_id_t id);
extern int pnet_node_attach(net_node_t node, net_id_t id, net_node_t parent);

extern net_node_t pnet_node_alloc(net_addr_t addr, pnet_proto_t proto);
extern net_node_t pnet_node_init(net_node_t node, net_addr_t addr, pnet_proto_t proto);


#endif /* PNET_NODE_H_ */
