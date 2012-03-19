/**
 * @file
 * @brief Network core
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/netfilter/match_lin.h>
#include <pnet/repo.h>
#include <pnet/graph.h>
#include <mem/objalloc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(net_core_init);

static int net_core_init(void) {
	net_node_t devs = pnet_get_module("devs entry");
	net_node_t gate = pnet_get_module("linux gate");

	struct pnet_graph *graph = pnet_graph_create();

	pnet_graph_add_src(graph, devs);

	pnet_graph_add_node(graph, gate);

	pnet_node_link(devs, gate);

	pnet_graph_start(graph);

	return 0;
}


net_node_t pnet_get_dev_by_device(struct net_device *dev) {
	net_node_t node = dev->pnet_node;

	if (NULL == node) {
		return pnet_get_module("devs entry");
	}

	return node;
}
