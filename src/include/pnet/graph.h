/**
 * @file
 *
 * @brief
 *
 * @date 01.11.2011
 * @author Anton Bondarev
 */

#ifndef PNET_GRAPH_H_
#define PNET_GRAPH_H_

struct pnet_graph {
	int id;
};


extern struct pnet_graph *pnet_get_graph(int sock);

extern int pnet_graph_start(struct pnet_graph *graph);

extern int pnet_graph_add_src(struct pnet_graph *graph, struct net_node *src);

extern int 	pnet_node_link(struct net_node *src, struct net_node *node);

#endif /* PNET_GRAPH_H_ */
