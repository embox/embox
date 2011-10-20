/**
 * @file
 * @brief Network types defs
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

//#include <net/skbuff.h>

#ifndef _NET_TYPES_H
#define _NET_TYPES_H

typedef int net_addr_t;
typedef int net_id_t;

typedef struct net_node *net_node_t;
typedef struct net_packet *net_packet_t;

typedef	int (*net_hnd)(net_packet_t pack);

typedef struct net_proto {
	net_id_t proto_id;
	net_hnd rx_hnd;
	net_hnd tx_hnd;
} *net_proto_t;

#define CHILD_CNT 0x10

struct net_node {
	int id;
	net_addr_t node_addr;
	net_proto_t proto;
	net_node_t parent;
	net_node_t children[CHILD_CNT];
};

#define SOCK_BUF_LEN 0x20

typedef struct net_socket {
	struct net_node node;
	char buf[SOCK_BUF_LEN];
} *net_socket_t;

enum net_packet_dir {
	NET_PACKET_RX,
	NET_PACKET_TX
};

struct net_packet {
	enum net_packet_dir dir;

	net_node_t node;

	int len;

	void *orig_data; /* this holds original data
			   while *data can be offsetted
			   free packet mem from here */
	void *data;
};

typedef struct net_dev *net_dev_t;

typedef int (*net_dev_op)(net_packet_t pack, net_dev_t dev);

typedef struct net_dev_ops {
	net_dev_op tx;
} *net_dev_ops_t;

typedef struct net_dev {
	net_node_t node;
	net_dev_ops_t ops;
} *net_dev_t;

#endif
