/**
 * @file
 *
 * @brief
 *
 * @date 31.10.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <embox/cmd.h>

#include <net/socket.h>
#include <pnet/repo.h>
#include <pnet/types.h>
#include <pnet/graph.h>

EMBOX_CMD(bt_main);

static int bt_main(int argc, char **argv) {
	struct pnet_graph *graph ;
	struct net_node *node, *src, *dc_hnd;

	graph = pnet_graph_create();

	src = pnet_get_module("lego_blue_core");
	pnet_graph_add_src(graph, src);

	node = pnet_get_module("nxt direct src");
	pnet_graph_add_node(graph, node);
	pnet_node_link(src, node);

	dc_hnd = pnet_get_module("nxt direct handler");
	pnet_graph_add_node(graph, dc_hnd);
	pnet_node_link(node, dc_hnd);

	pnet_graph_start(graph);

	return 0;
}
