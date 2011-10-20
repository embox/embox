/**
 * @file
 * @brief Network core interface
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#ifndef _NET_CORE_H
#define _NET_CORE_H

#include <net/types.h>

extern int net_proto_init(net_proto_t proto, net_id_t id, net_hnd rx, net_hnd tx);

extern int node_attach(net_node_t node, net_id_t id, net_node_t parent);

extern net_node_t net_node_alloc(net_addr_t addr, net_proto_t proto);

extern net_packet_t net_pack_alloc(net_node_t node, enum net_packet_dir dir, void *data, int len);

extern int net_pack_free(net_packet_t pack);

extern int net_core_send(net_node_t node, void *data, int len);

extern int net_core_receive(net_node_t  node, void *data, int len);

extern net_dev_t net_dev_register(net_dev_ops_t dev_ops);


////
extern net_socket_t net_socket_open(net_id_t id, net_node_t parent);

#endif
