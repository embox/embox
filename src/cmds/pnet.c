 /**
 * @file
 * @brief
 *
 * @date 14.03.2012
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <net/in.h>

#include <pnet/netfilter/match_lin.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/graph.h>
#include <string.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: pnet [-h] [-p graph] [-g] [-r graph] [-s graph]");
	printf("[-l node1 node2] [(-a | -d) --node graph node] [(-a | -d) --rule graph [-ip ip] [-proto proto]]");
	printf("[-m [--enable | --disable] rule \n");
}

extern struct list_head graphs_list;

static void print_graph_names(void) {
	struct pnet_graph *gr;

	printf("%s", "Running graphs: ");
	list_for_each_entry(gr, &graphs_list, lnk) {
		if (PNET_GRAPH_STARTED == gr->state)
			printf("%s, ", gr->name);
	}
	printf("\n%s", "Stopped graphs: ");
	list_for_each_entry(gr, &graphs_list, lnk) {
		if (PNET_GRAPH_STOPPED == gr->state)
			printf("%s, ", gr->name);
	}
}

static void print_graph_nodes(struct pnet_graph *gr) {
	net_node_t node;

	list_foreach(node, &gr->nodes, gr_link) {
		printf("%s, ", node->proto->name);
	}
}
//FIXME now this function return only first graph in list if exist
static struct pnet_graph *get_graph_by_name(char *name) {
	struct pnet_graph *gr;

	list_for_each_entry(gr, &graphs_list, lnk) {
		if (0 == strcmp(gr->name, name))
			return gr;
	}

	return NULL;
}

static char *next_word(char *str) {
	while (*str != '\0')
		str++;

	return ++str;
}

static int add_node(char *str) {
	char *name;
	struct pnet_graph *gr;
	net_node_t node;

	if(NULL != (name = str))
		gr = get_graph_by_name(name);
	else
		return -ENOENT;

	if(NULL != (name = next_word(name)))
		node = pnet_get_module(name);
	else
		return -ENOENT;

	return pnet_graph_add_node(gr, node);
}

static int link_node(char *str) {
	char *name;
	net_node_t src, node;

	if(NULL != (name = str))
		src = pnet_get_module(name);
	else
		return -ENOENT;

	if(NULL != (name = next_word(name)))
		node = pnet_get_module(name);
	else
		return -ENOENT;

	return pnet_node_link(src, node);
}

static int exec(int argc, char **argv) {
	int opt;
	char *name;
	struct pnet_graph *gr;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hgprsa:l:"))) {
		name = optarg;
		switch(opt) {
		case 'h':
			print_usage();
			break;
		case 'g':
			print_graph_names();
			break;
		case 'p':
			sscanf(optarg, "%s", name);
			if (NULL != (gr = get_graph_by_name(name))){
				printf("nodes of %s: ", gr->name);
				print_graph_nodes(gr);
			} else {
				printf("%s: no such graph", name);
			}
			break;
		case 'r':
			sscanf(optarg, "%s", name);
			if (NULL != (gr = get_graph_by_name(name))) {
				pnet_graph_start(gr);
				printf("%s run", gr->name);
			} else {
				printf("%s: no such graph", name);
			}
			break;
		case 's':
			sscanf(optarg, "%s", name);
			if (NULL != (gr = get_graph_by_name(name))) {
				pnet_graph_stop(gr);
				printf("%s stopped", gr->name);
			} else {
				printf("%s: no such graph", name);
			}
			break;
		case 'a' :
			if (!strcmp("--node", name)) {
				name = next_word(name);
				if (add_node(name) < 0)
					printf("%s", "error\n");
			} else {
				print_usage();
			}
			break;
		case 'l':
			if (!strcmp("--node", name)) {
				name = next_word(name);
				if(link_node(name) < 0)
					printf("%s", "error");
			} else {
				print_usage();
			}
			break;
		default:
			return 0;
		}
	}

	return 0;
}
