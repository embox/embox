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
	printf("[-l graph node1 node2] [(-a | -d) --node graph node] [(-a | -d) --rule graph [-ip ip] [-proto proto]]");
	printf("[-m [--enable | --disable] rule \n");
}

extern struct list_head graphs_list;

//FIXME now this function return only first graph in list if exist
static struct pnet_graph *get_graph_by_name(char *name) {
	struct pnet_graph *gr;

	list_for_each_entry(gr, &graphs_list, lnk) {
		if (0 == strcmp(gr->name, name))
			return gr;
	}

	return NULL;
}

static net_node_t get_node_from_graph_by_name(struct pnet_graph *gr, char *name) {
	net_node_t node;

	list_foreach(node, &gr->nodes, gr_link) {
		if(!strcmp(node->proto->name, name))
			return node;
	}

	return NULL;
}

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
	net_node_t node, root;
	net_node_matcher_t matcher;
	match_rule_t rule;
	struct list_head *h;

	root = list_first_element(&gr->nodes, struct net_node, gr_link);
	node = root->rx_dfault;

	printf("%s\n", root->proto->name);
	while (NULL != node) {
		/* FIXME matcher must have special type to differ from other nodes in graph */
		if (strcmp(node->proto->name, "matcher")) {
			printf("%s\n ", node->proto->name);
		} else {
			/* if node is matcher it contain links with some other nodes */
			matcher = (net_node_matcher_t) node;
			printf("%s->", matcher->node.proto->name);

			list_for_each (h, &matcher->match_rx_rules) {
				rule = member_cast_out(h, struct match_rule, lnk);
				printf("%s, ", rule->next_node->proto->name);
			}
			printf("\n");
		}
		node = node->rx_dfault;
	}
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

	if(NULL == (name = str) || NULL == (gr = get_graph_by_name(name)))
		return -ENOENT;

	if(NULL == (name = next_word(name)) || NULL == (node = pnet_get_module(name)))
		return -ENOENT;

	return pnet_graph_add_node(gr, node);
}

static int link_node(char *str) {
	char *name;
	net_node_t src, node;
	struct pnet_graph *gr;

	if(NULL == (name = str) || NULL == (gr = get_graph_by_name(name)))
		return -ENOENT;

	if(NULL != (name = str) || NULL == (src = get_node_from_graph_by_name(gr, name)))
		return -ENOENT;

	if(NULL != (name = next_word(name)) || NULL == (node = pnet_get_module(name)))
		return -ENOENT;

	return pnet_node_link(src, node);
}

static int exec(int argc, char **argv) {
	int opt;
	char *name;
	struct pnet_graph *gr;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hgp:r:s:a:l:"))) {
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
				printf("nodes of %s:\n ", gr->name);
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
			} else if (!strcmp("--graph", name)) {
				name = next_word(name);
				pnet_graph_create(name);
			} else {
				print_usage();
			}
			break;
		case 'l':
			if(link_node(name) < 0)
				printf("%s", "error");
			break;
		default:
			return 0;
		}
	}

	return 0;
}
