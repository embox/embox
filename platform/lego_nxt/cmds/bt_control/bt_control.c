/**
 * @file
 * @brief
 *
 * @date 31.10.2011
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <embox/cmd.h>
#include <pnet/repo.h>
#include <pnet/types.h>
#include <pnet/graph.h>
#include <pnet/node/direct_comm.h>
#include <drivers/bluetooth/blue_core4.h>

EMBOX_CMD(bt_main);

static struct net_node *add_mod(const char *str_id, struct pnet_graph *graph, struct net_node *prev) {
	struct net_node *node = pnet_get_module(str_id);
	if (node == NULL) {
		return NULL;
	}
	pnet_graph_add_node(graph, node);
	pnet_node_link(prev, node);
	return node;
}

static int bt_main(int argc, char **argv) {
	struct pnet_graph *graph ;
	struct net_node *hw_data, *hw_ctrl, *bc_data, *bc_ctrl, *dc_format_data,
			*dc_format_ctrl, *dc_exec;

	graph = pnet_graph_create("bt lego");

	hw_data = pnet_get_module(BLUETOOTH_HW_BLUE_CORE4_DATA);
	pnet_graph_add_src(graph, hw_data);
	assert(hw_data);

	hw_ctrl = pnet_get_module(BLUETOOTH_HW_BLUE_CORE4_CTRL);
	pnet_graph_add_src(graph, hw_ctrl);
	assert(hw_ctrl);

	bc_data = add_mod(BLUETOOTH_DRV_BLUE_CORE4_DATA, graph, hw_data);
	bc_ctrl = add_mod(BLUETOOTH_DRV_BLUE_CORE4_CTRL, graph, hw_ctrl);
	assert(bc_data);
	assert(bc_ctrl);

	dc_format_data = add_mod(PNET_NODE_DIRECT_COMM_FORMATION_DATA, graph, bc_data);
	dc_format_ctrl = add_mod(PNET_NODE_DIRECT_COMM_FORMATION_CTRL, graph, bc_ctrl);
	assert(dc_format_data);
	assert(dc_format_ctrl);

	dc_exec = add_mod(PNET_NODE_DIRECT_COMM_EXECUTER, graph, dc_format_data);
	assert(dc_exec);

	return pnet_graph_start(graph);
}
