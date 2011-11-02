/**
 * @file
 *
 * @brief
 *
 * @date 02.11.2011
 * @author Anton Bondarev
 */

#include <errno.h>

#include <pnet/core.h>
#include <pnet/graph.h>

#include <mem/objalloc.h>

#define CONFIG_PNET_GRAPH_CNT 0x1

OBJALLOC_DEF(graphs, struct pnet_graph, CONFIG_PNET_GRAPH_CNT);

struct pnet_graph *pnet_get_graph(int sock) {
	return (struct pnet_graph *) objalloc(&graphs);
}
struct pnet_graph *pnet_graph_create(void) {
	return (struct pnet_graph *) objalloc(&graphs);
}

int pnet_graph_start(struct pnet_graph *graph) {
	if (graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	graph->state = PNET_GRAPH_STARTED;

	return 0;
}

int pnet_graph_stop(struct pnet_graph *graph) {
	if (graph->state != PNET_GRAPH_STARTED) {
		return -EINVAL;
	}

	graph->state = PNET_GRAPH_STOPPED;

	return 0;
}


int pnet_graph_add_src(struct pnet_graph *graph, struct net_node *src) {
	return pnet_graph_add_node(graph, src);
}

int pnet_graph_add_node(struct pnet_graph *graph, struct net_node *node) {
	if (graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	node->graph = graph;

	return 0;
}

int pnet_node_link(struct net_node *src, struct net_node *node) {
	if (src->graph != node->graph || src->graph != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	pnet_node_attach(src, NET_RX_DFAULT, node);

	return 0;
}

