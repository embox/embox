 /**
 * @file
 * @brief
 *
 * @date 14.03.2012
 * @author Alexander Kalmuk
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <net/in.h>

#include <pnet/netfilter/match_lin.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/graph.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: pnet [-h] [-p] [-g] \n");
}

extern struct list_head graphs_list;

static void print_graph_names(void) {
	struct pnet_graph *gr;

	printf("%s", "Running graphs: ");
	list_for_each_entry(gr, &graphs_list, lnk) {
		if(PNET_GRAPH_STARTED == gr->state)
			printf("%s, ", gr->name);
	}
	printf("\n%s", "Stopped graphs: ");
	list_for_each_entry(gr, &graphs_list, lnk) {
		if(PNET_GRAPH_STOPPED == gr->state)
			printf("%s, ", gr->name);
	}
}

static void print_graph_nodes(struct pnet_graph *gr) {
	net_node_t node;

	list_foreach(node, &gr->nodes, gr_link) {
		printf("%s, ", node->proto->name);
	}
}

static int exec(int argc, char **argv) {
	int opt;
	char *name;
	struct pnet_graph *gr;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hgpr:s:"))) {
		name = optarg;
		switch(opt) {
		case 'h':
			print_usage();
			break;
		case 'g':
			print_graph_names();
			break;
		case 'p':
			list_for_each_entry(gr, &graphs_list, lnk) {
				printf("nodes of %s: ", gr->name);
				print_graph_nodes(gr);
				printf("\n");
			}
			break;
		case 'r':
			sscanf(optarg, "%s", name);
			list_for_each_entry(gr, &graphs_list, lnk) {
				if(0 == strcmp(gr->name, name))
					pnet_graph_start(gr);
			}
			break;
		case 's':
			sscanf(optarg, "%s", name);
			list_for_each_entry(gr, &graphs_list, lnk) {
				if(0 == strcmp(gr->name, name))
					pnet_graph_stop(gr);
			}
			break;
		default:
			return 0;
		}
	}

	return 0;
}
