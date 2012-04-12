/**
 * @file
 *
 * @brief Graph executer process
 *
 * @date 14.03.2012
 * @author Anton Bondarev
 */
#include <errno.h>

#include <pnet/core.h>
#include <pnet/graph.h>
#include <pnet/pnet_pack.h>
#include <pnet/node.h>

static int step_process(struct pnet_pack *pack, net_hnd hnd, net_node_t next_node) {
	net_node_t node;
	net_id_t res = NET_HND_DFAULT;

	assert(pack);
	assert(pack->node);

	node = pack->node;

	if (0 != pnet_graph_run_valid(node->graph) && !node_is_supporter(node)) {
		pnet_pack_destroy(pack);
		return -EINVAL;
	}

	if (node->proto != NULL) {
		if(NULL != hnd) {
			res = hnd(pack);
		}
	}

	if (res & NET_HND_DFAULT) {
		assert(next_node);
		pack->node = next_node;
	}

	if (res != NET_PACK_ACCEPTED) {
		pnet_rx_thread_add(pack);
	}

	return 0;
}

int pnet_process(struct pnet_pack *pack) {
	if (pack->dir == PNET_PACK_DIRECTION_RX) {
		return step_process(pack, pnet_proto_rx_hnd(pack->node), pack->node->rx_dfault);
	}
	return step_process(pack, pnet_proto_tx_hnd(pack->node), pack->node->tx_dfault);
}
