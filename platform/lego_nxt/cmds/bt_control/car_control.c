/**
 * @file
 *
 * @brief
 *
 * @date 03.11.2011
 * @author Anton Bondarev
 */

EMBOX_CMD(bt_main);

static int bt_main(int argc, char **argv) {
	struct pnet_graph *graph ;
	struct net_node *node_prev, *node_next;

	graph = pnet_graph_create();

	node_next = pnet_get_module("lego_blue_core");
	pnet_graph_add_node(graph, node_next);

	node_next = pnet_get_module("lego_direct");
	pnet_graph_add_node(graph, node_next);

	pnet_node_link(node_prev, node_next);

	node_prev = node_next;

	node_next = pnet_get_module("nxt direct handler");
	pnet_graph_add_node(graph, node_next);
	pnet_node_link(node_prev, node_next);

	pnet_graph_start(graph);

	return 0;
}
