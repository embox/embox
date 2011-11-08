/**
 * @file
 * @brief Working matcher example.
 * Ping embox and match NET packets:
 * if IP packet then devs->matcher->info printer->linux gate
 * if ARP packet then devs->matcher->linux gate
 *
 * where *->*-> is packet way through packet handlers.
 *
 * @date 26.10.2011
 * @author Alexander Kalmuk
 */


#include <pnet/match.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/graph.h>

#include <framework/example/self.h>
EMBOX_EXAMPLE(match_example);

static int match_example(int argc, char **argv) {
	struct pnet_graph *graph;
	match_rule_t new_rule;
	net_node_t match_node;
	net_node_t devs;
	net_node_t lin_gate;
	net_node_t info;

	/* create nodes to add them in graph*/
	devs = pnet_get_module("devs entry");
	lin_gate = pnet_get_module("linux gate");
	info = pnet_get_module("info printer");
	match_node = pnet_get_module("matcher");

	/* create graph*/
	graph =  pnet_graph_create();
	/* add nodes */
	pnet_graph_add_node(graph, devs);
	pnet_graph_add_node(graph, lin_gate);
	pnet_graph_add_node(graph, info);
	pnet_graph_add_node(graph, match_node);

	pnet_node_link(devs, match_node);
	pnet_node_link(match_node, lin_gate);
	pnet_node_link(info, lin_gate);

	/* create rule */
	new_rule = pnet_rule_alloc();
	new_rule->next_node = info;

	pnet_graph_start(graph);

	return 0;
}
