/**
 * @file
 *
 * @brief Utility functions for working with 'pnet' graphs
 *
 * @date 02.11.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <mem/objalloc.h>

#include <pnet/core.h>
#include <pnet/graph.h>
#include <pnet/node.h>

OBJALLOC_DEF(graphs, struct pnet_graph, CONFIG_PNET_GRAPH_CNT);

LIST_HEAD(pnet_graphs);

struct pnet_graph *pnet_get_graph(int sock) {
	return (struct pnet_graph *) objalloc(&graphs);
}

struct pnet_graph *pnet_graph_create(char *name) {
	struct pnet_graph *gr;

	list_for_each_entry(gr, &pnet_graphs, lnk) {
		if(!strcmp(gr->name, name))
			return NULL;
	}

	if (NULL == (gr = (struct pnet_graph *) objalloc(&graphs)))
		return NULL;

	INIT_LIST_HEAD(&gr->nodes);
	INIT_LIST_HEAD(&gr->lnk);
	list_add_tail(&gr->lnk, &pnet_graphs);

	strcpy(gr->name, name);
	gr->state = PNET_GRAPH_STOPPED;

	return gr;
}

int pnet_graph_start(struct pnet_graph *graph) {
	net_node_t node = NULL;
	net_node_hnd hnd;

	assert(graph);

	if (graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	list_for_each_entry(node, &graph->nodes, gr_link) {
		if(NULL != (hnd = pnet_proto_start(node))) {
			hnd(node);
		}
	}

	graph->state = PNET_GRAPH_STARTED;

	return 0;
}

int pnet_graph_stop(struct pnet_graph *graph) {
	net_node_t node = NULL;
	net_node_hnd hnd;

	assert(graph);

	if (graph->state != PNET_GRAPH_STARTED) {
		return -EINVAL;
	}

	list_for_each_entry(node, &graph->nodes, gr_link) {
		if(NULL != (hnd = pnet_proto_stop(node))) {
			hnd(node);
		}
	}

	graph->state = PNET_GRAPH_STOPPED;

	return 0;
}

int pnet_graph_add_src(struct pnet_graph *graph, struct net_node *src) {
	return pnet_graph_add_node(graph, src);
}

int pnet_graph_add_node(struct pnet_graph *graph, struct net_node *node) {
	assert(graph);
	assert(node);

	if (graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	if (NULL != node->graph) {
		return -EBUSY;
	}

	node->graph = graph;

	list_add_tail(&node->gr_link, &graph->nodes);

	return 0;
}

int pnet_node_link(struct net_node *src, struct net_node *node) {
	assert(src);
	assert(node);

	if (src->graph != node->graph || src->graph->state != PNET_GRAPH_STOPPED) {
		return -EINVAL;
	}

	pnet_node_attach(src, NET_RX_DFAULT, node);

	return 0;
}
