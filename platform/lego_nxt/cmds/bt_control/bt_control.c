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

EMBOX_CMD(main);

static int main(int argc, char **argv) {
	struct sock sock;
	struct pnet_graph graph;
	struct pnet_node node, src;

	if (sock = socket(PNET_GRAPH, 0, 0)) {
		//error
	}
	graph = pnet_get_graph(sock);

	src = pnet_get_node("lego_blue_core");
	pnet_add_src(graph, src);
	node = pnet_get_node("lego_direct");
	pnet_link(src, node);

	pnet_graph_start(graph);

	return 0;
}
