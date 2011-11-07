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
#include <net/netdevice.h>
#include <util/list.h>

typedef int net_addr_t;
typedef int net_id_t;
typedef char net_prior_t;

struct net_node;
struct pnet_graph;

typedef struct net_packet *net_packet_t;

typedef int (*net_hnd)(net_packet_t pack);
typedef int (*net_node_hnd)(struct net_node *node);
typedef struct net_node *(*net_alloc_hnd)(void);

typedef struct pnet_proto {
	const char *name;
	net_hnd rx_hnd;
	net_hnd tx_hnd;
	net_alloc_hnd alloc;
	net_node_hnd free;
	net_node_hnd start;
	net_node_hnd stop;
} *pnet_proto_t;

struct net_node {
	struct net_node *this;
	struct pnet_graph *graph;
	struct list_link gr_link;
	struct pnet_proto *proto;

	struct net_node *tx_dfault;
	struct net_node *rx_dfault;

	net_prior_t prior;
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

struct pnet_dev {
	struct net_node node;
	struct net_device *dev;

};

#include __module_headers(embox/pnet/core/pack/api)

#endif
