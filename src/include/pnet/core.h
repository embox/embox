/**
 * @file
 * @brief Network core interface
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#ifndef PNET_CORE_H_
#define PNET_CORE_H_

#include <pnet/types.h>

#define PNET_PRIORITY_COUNT 0x10

#define NET_RX_DFAULT -1

#define NET_TX_DFAULT -2

#define NET_HND_DFAULT     (0x01L << 0)
#define NET_HND_SUPPRESSED (0x01L << 1)

extern int pnet_proto_init(pnet_proto_t proto, net_id_t id, net_hnd rx, net_hnd tx);

extern net_node_t pnet_node_get(net_node_t node, net_id_t id);
extern int pnet_node_attach(net_node_t node, net_id_t id, net_node_t parent);

extern net_node_t pnet_node_alloc(net_addr_t addr, pnet_proto_t proto);
extern net_node_t pnet_node_init(net_node_t node, net_addr_t addr, pnet_proto_t proto);

//extern net_packet_t pnet_pack_alloc(net_node_t node, int len);
//extern net_packet_t pnet_pack_alloc_skb(net_node_t node, struct sk_buff *skb);
//extern int pnet_pack_free(net_packet_t pack);

extern int pnet_path_set_prior(net_node_t node, net_prior_t prior);

extern int pnet_entry(struct pnet_pack *pack);
extern int pnet_process(struct pnet_pack * pack);

extern net_node_t pnet_dev_register(struct net_device *dev);
extern struct net_device *pnet_get_net_device(net_node_t node);

extern net_node_t pnet_dev_get_entry(void);

extern int pnet_rx_thread_add(struct pnet_pack * pack);

extern net_node_t pnet_get_dev_by_device(struct net_device *dev);

#endif /* PNET_CORE_H_ */
