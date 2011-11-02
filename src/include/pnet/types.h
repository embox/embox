/**
 * @file
 * @brief Network types defs
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

//#include <net/skbuff.h>
#ifndef _PNET_TYPES_H
#define _PNET_TYPES_H

#include <net/in.h>
#include <net/if_ether.h>
#include <lib/list.h>

typedef int net_addr_t;
typedef int net_id_t;
typedef char net_prior_t;

struct net_node;
struct pnet_graph;

typedef struct net_packet *net_packet_t;

typedef int (*net_hnd)(net_packet_t pack);
typedef int (*net_node_free_hnd)(struct net_node *node); /* destructor  */
typedef int (*net_node_init_hnd)(struct net_node *node); /* initializer */

typedef struct pnet_proto {
	net_id_t proto_id;
	net_hnd rx_hnd;
	net_hnd tx_hnd;
	net_node_init_hnd init;
	net_node_free_hnd free;
} *pnet_proto_t;

struct net_node {
	int id;
	struct pnet_graph *graph;
	net_prior_t prior;
	net_addr_t node_addr;
	struct pnet_proto *proto;
	struct net_node *tx_dfault;
	struct net_node *rx_dfault;
};
typedef struct net_node *net_node_t;

enum net_packet_dir {
	NET_PACKET_RX, NET_PACKET_TX
};

struct net_dev;
typedef int (*net_dev_op)(net_packet_t pack, struct net_dev *dev);

typedef struct net_dev_ops {
	net_dev_op tx;
}*net_dev_ops_t;

struct net_dev {
	net_node_t node;
	int id;
	net_dev_ops_t ops;
};
typedef struct net_dev *net_dev_t;

#include __module_headers(embox/pnet/core/pack/api)

#endif
