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

typedef struct net_packet *net_packet_t;

typedef int (*net_hnd)(net_packet_t pack);

typedef struct net_proto {
	net_id_t proto_id;
	net_hnd rx_hnd;
	net_hnd tx_hnd;
}*net_proto_t;

#define CHILD_CNT 0x10

struct net_node {
	int id;
	net_prior_t prior;
	net_addr_t node_addr;
	struct net_proto *proto;
	struct net_node *parent;
	struct net_node *children[CHILD_CNT];
	struct net_node *dfault;
	struct list_head match_rx_rules;
	struct list_head match_tx_rules;
};
typedef struct net_node *net_node_t;

#define SOCK_BUF_LEN 0x20

typedef struct net_socket {
	struct net_node node;
	char buf[SOCK_BUF_LEN];
}*net_socket_t;

enum net_packet_dir {
	NET_PACKET_RX, NET_PACKET_TX
};

struct sk_buff;

struct net_packet {
	enum net_packet_dir dir;

	net_node_t node;

	int len;

	void *orig_data; /* this holds original data
	 while *data can be offsetted
	 free packet mem from here */
	void *data;

	struct sk_buff *skbuf;
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

struct match_rule {
	struct ethhdr ether_header;
	in_addr_t dest_ip;
	in_addr_t src_ip;
	uint8_t proto;
	uint16_t dest_port;
	uint16_t src_port;
	int next_node;
	struct list_head lnk;
};

typedef struct match_rule match_rule_t;

#endif
