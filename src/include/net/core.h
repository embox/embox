/**
 * @file
 * @brief Network core interface
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

extern int net_proto_init(net_proto_t proto, net_id_t id, net_hnd rx, net_hnd tx);
extern int node_attach(net_node_t node, net_id_t id, net_node_t parent);
extern net_node_t net_node_alloc(net_addr_t addr, net_proto_t proto);
