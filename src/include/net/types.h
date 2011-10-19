/**
 * @file
 * @brief Network types defs
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <net/skbuff.h>

typedef net_addr_t int;
typedef net_id_t int;

typedef struct net_packet {
	enum {
		NET_PACKET_RX,
		NET_PACKET_TX
	} type;

	net_node_t;

	void* data;
}

typedef	(*net_hnd)(net_node_t this, void* data);

typedef struct net_proto {
	net_id_t proto_id;
	net_hnd rx_hnd;
	net_hnd tx_hnd;
} net_proto_t;

#define CHILD_CNT 0x10

typedef struct net_node {
	int id;
	net_addr_t node_addr;
	net_proto_t proto;
	net_node_t children[CHILD_CNT];
} net_node_t;
