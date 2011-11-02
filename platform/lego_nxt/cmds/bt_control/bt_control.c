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
	int sock;
	struct pnet_graph *graph;
	struct net_node *node, *src;

	if (-1 == (sock = socket(PNET_GRAPH, 0, 0))) {
		//error
	}

	graph = pnet_get_graph(sock);

	src = pnet_get_module("lego_blue_core");

	pnet_graph_add_src(graph, src);

	node = pnet_get_module("lego_direct");

	pnet_graph_add_node(node);

	pnet_node_link(src, node);

	pnet_graph_start(graph);

	return 0;
}
