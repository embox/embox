/**
 *
 */

#include <pnet/match.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/graph.h>

#include <util/array.h>
#include <embox/unit.h>
#include <ctype.h>
#include <framework/cmd/api.h>

EMBOX_UNIT_INIT(graph);

static int graph(void) {
	struct pnet_graph *graph;
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

	/* You can also create rules in pnet_rules.inc.
	 * They will be initialize in rules_init function. See pnet/rules_init.c
	 * for more details */

	/* now graph can packets handle */
	pnet_graph_start(graph);

	return 0;
}
