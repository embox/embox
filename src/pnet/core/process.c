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
	net_id_t res = NET_HND_FORWARD_DEFAULT;

	assert(pack);
	assert(pack->node);

	node = pack->node;

	if (0 != pnet_graph_run_valid(node->graph)) {
		pnet_pack_destroy(pack);
		return -EINVAL;
	}

	if (node->proto != NULL) {
		if(NULL != hnd) {
			res = hnd(pack);
		}
	}

	switch (res) {
	case NET_HND_FORWARD_DEFAULT:
		assert(next_node);
		pack->node = next_node;
		/* FALLTHROUGH */
	case NET_HND_FORWARD:
		pnet_rx_thread_add(pack);
		break;
	case NET_HND_STOP_FREE:
		pnet_pack_destroy(pack);
		break;
	case NET_HND_STOP:
		break;
	default:
		/* do nothing */
		break;
	}

	return 0;
}

int pnet_process(struct pnet_pack *pack) {
	if (pack->dir == PNET_PACK_DIRECTION_RX) {
		return step_process(pack, pnet_proto_rx_hnd(pack->node), pack->node->rx_dfault);
	}
	return step_process(pack, pnet_proto_tx_hnd(pack->node), pack->node->tx_dfault);
}
