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

static struct net_node *add_mod(const char *str_id, struct pnet_graph *graph, struct net_node *prev) {

	struct net_node *node = pnet_get_module(str_id);
	assert(node != NULL);
	pnet_graph_add_node(graph, node);
	pnet_node_link(prev, node);
	return node;
}

static int bt_main(int argc, char **argv) {
	struct pnet_graph *graph ;
	struct net_node *hw_data, *hw_ctrl, *bc_data, *bc_ctrl, *dc_format_data;
	//		*dc_format_ctrl, *dc_exec;

	graph = pnet_graph_create();

	hw_data = pnet_get_module("bt hw data");
	pnet_graph_add_src(graph, hw_data);

	hw_ctrl = pnet_get_module("bt hw ctrl");
	pnet_graph_add_src(graph, hw_ctrl);

	bc_data = add_mod("blue_core data", graph, hw_data);
	bc_ctrl = add_mod("blue_core ctrl", graph, hw_ctrl);

	dc_format_data = add_mod("direct_comm formation data", graph, bc_data);
	add_mod("direct_comm formation ctrl", graph, bc_ctrl);

	add_mod("direct_comm exec", graph, dc_format_data);

	return pnet_graph_start(graph);

}
