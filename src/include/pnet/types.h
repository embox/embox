/**
 * @file
 * @brief pnet types definitions
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#ifndef PNET_TYPES_H_
#define PNET_TYPES_H_

#include <stdint.h>
#include <linux/list.h>
#include <sys/time.h>
#include <kernel/time/ktime.h>


typedef int net_addr_t;
typedef int net_id_t;
typedef char net_prior_t;

struct net_node;
struct pnet_graph;
struct pnet_proto;

typedef struct net_packet *net_packet_t;

struct net_node {
	const char *name;		/*< unique name inside graph */
	struct pnet_graph *graph;
	struct list_head gr_link;
	struct pnet_proto *proto;

	struct net_node *tx_dfault;
	struct net_node *rx_dfault;

	net_prior_t prior;
};
typedef struct net_node *net_node_t;

enum PNET_PACK_DIRECTION {
	PNET_PACK_DIRECTION_RX,
	PNET_PACK_DIRECTION_TX
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

struct pnet_pack_stat {
	/* TODO struct timespec */
	clock_t start_time;
	clock_t running_time;
	int interrupt_count;
	clock_t last_sync;
};

struct pnet_pack {
	uint32_t type;
	uint32_t priority;
	struct net_node *node;
	enum PNET_PACK_DIRECTION dir;
	void *data;
	struct pnet_pack_stat stat;
	struct list_head link;
};

#include <pnet/proto.h>

#endif /* PNET_TYPES_H_ */
