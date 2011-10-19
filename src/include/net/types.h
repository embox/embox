/**
 * @file
 * @brief Network types defs
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

//#include <net/skbuff.h>

typedef int net_addr_t;
typedef int net_id_t;

typedef struct net_node *net_node_t;

typedef	int (*net_hnd)(net_node_t this, void* data);

typedef struct net_proto {
	net_id_t proto_id;
	net_hnd rx_hnd;
	net_hnd tx_hnd;
} *net_proto_t;

#define CHILD_CNT 0x10

typedef struct net_node {
	int id;
	net_addr_t node_addr;
	net_proto_t proto;
	net_node_t children[CHILD_CNT];
} *net_node_t;

typedef struct net_socket {
	struct net_node node;
} *net_socket_t;

typedef struct net_packet {
	enum {
		NET_PACKET_RX,
		NET_PACKET_TX
	} type;

	net_node_t node;

	void* data;
} *net_packet_t;
