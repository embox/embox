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

#include <util/fun_call.h>

#include <mem/objalloc.h>

#define CONFIG_PNET_GRAPH_CNT 0x10

OBJALLOC_DEF(graphs, struct pnet_graph, CONFIG_PNET_GRAPH_CNT);

struct pnet_graph *pnet_get_graph(int sock) {
	return (struct pnet_graph *) objalloc(&graphs);
}
struct pnet_graph *pnet_graph_create(void) {
	struct pnet_graph *gr = (struct pnet_graph *) objalloc(&graphs);
	list_init(&gr->nodes);
	gr->state = PNET_GRAPH_STOPPED;

	return gr;
}

int pnet_graph_start(struct pnet_graph *graph) {
	net_node_t node = NULL;
	if (graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	list_foreach(node, &graph->nodes, gr_link) {
		fun_call(pnet_proto_start(node), node);
	}

	graph->state = PNET_GRAPH_STARTED;

	return 0;
}

int pnet_graph_stop(struct pnet_graph *graph) {
	net_node_t node = NULL;
	if (graph->state != PNET_GRAPH_STARTED) {
		return -EINVAL;
	}

	list_foreach(node, &graph->nodes, gr_link) {
		fun_call(pnet_proto_stop(node), node);
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

	list_add_last_element(node, &graph->nodes, gr_link);

	return 0;
}

int pnet_node_link(struct net_node *src, struct net_node *node) {
	if (src->graph != node->graph || src->graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	pnet_node_attach(src, NET_RX_DFAULT, node);

	return 0;
}

