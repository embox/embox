/**
 * @file
 * @brief
 *
 * @date 01.11.2011
 * @author Anton Bondarev
 */

#ifndef PNET_GRAPH_H_
#define PNET_GRAPH_H_

#include <pnet/types.h>
#include <mem/objalloc.h>
#include <linux/list.h>

#define MAX_GRAPH_NAME_SIZE 20

extern struct list_head pnet_graphs;

enum pnet_graph_state {
	PNET_GRAPH_STOPPED,
	PNET_GRAPH_STARTED
};

struct pnet_graph {
	char name[MAX_GRAPH_NAME_SIZE];
	int id;
	enum pnet_graph_state state;
	struct list_head nodes;
	struct list_head lnk;
};

extern struct pnet_graph *pnet_get_graph(int sock);

extern struct pnet_graph *pnet_graph_create(char *name);

extern int pnet_graph_start(struct pnet_graph *graph);
extern int pnet_graph_stop(struct pnet_graph *graph);

extern int pnet_graph_add_src(struct pnet_graph *graph, struct net_node *src);

extern int pnet_graph_add_node(struct pnet_graph *graph, struct net_node *node);

extern int pnet_node_link(struct net_node *src, struct net_node *node);

static inline int pnet_graph_run_valid(struct pnet_graph *graph) {
	return !((graph != NULL) && (graph->state == PNET_GRAPH_STARTED));
}

#endif /* PNET_GRAPH_H_ */
