 /**
 * @file
 * @brief Util for PNET configuration
 *
 * @date 14.03.2012
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <err.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <net/util.h>
#include <net/in.h>

#include <pnet/netfilter/match_lin.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/graph.h>


EMBOX_CMD(exec);

extern struct pnet_module __pnet_mod_repo[];

#define MAX_WORD_LENGTH 100
#define MAX_TOKEN_COUNT 10
#define RULE_OPTION_QUANTITY 3

typedef int (*_rule_setter) (struct pnet_graph *gr, match_rule_t rule, char *rule_str);

static int print_graph(char **argv);
static void print_list_of_graph_nodes(struct pnet_graph *gr);
static void print_graph_names(void);
static void print_rules(net_node_matcher_t node);

static int add_node(char **argv);
static int link_node(char **argv);
static int unlink_node(char **argv);

static int rule_set_mac(struct pnet_graph *gr, match_rule_t rule, char *rule_str);
static int rule_set_ip(struct pnet_graph *gr, match_rule_t rule, char *rule_str);
static int rule_set_next_node(struct pnet_graph *gr, match_rule_t rule, char *rule_str);
static match_rule_t rule_get_by_id(net_node_t node, char id);

/* macros to get graph, node or rule by it name */

#define get_graph(graph, name, error) \
		if (NULL == (graph = get_graph_by_name(name))) { \
			printf("%s: no such graph ", name);		 \
			return -error;								 \
		}

#define get_node_from_graph(graph, node, node_name, error) \
		if (NULL == (node = get_node_from_graph_by_name(graph, node_name))) {   \
			printf("%s: no such node in graph '%s'", node_name, graph->name); \
			return -error;														\
		}

#define get_node_from_repo(node, name, error) \
		if (NULL == (node = pnet_get_module(name))) { \
			printf("%s: no such node ", name);	  \
			return -error;							  \
		}

#define get_rule_from_node(node, rule, rule_name, error) \
		if (NULL == (rule = rule_get_by_id(node, *rule_name))) { \
			printf("%s: no such rule ", rule_name);				 \
			return -error;										 \
		}

static int delete_brokens(char *str);

struct rule {
	char *option;
	_rule_setter setter;
};

static struct rule rule_setters[RULE_OPTION_QUANTITY] = {
		{.option = "--mac",    .setter = rule_set_mac},
		{.option = "--ip",     .setter = rule_set_ip},
		{.option = "--node",   .setter = rule_set_next_node},
};

static void print_usage(void) {
	printf("Usage: pnet [options] [keys] graph [nodes | rules]\n");
}

//TODO move from here
static int delete_brokens(char *str) {
	int len;
	char *cur;
	int overlooking_words_cnt;

	overlooking_words_cnt = 0;
	len = MAX_WORD_LENGTH;
	cur = str;
	cur++;

	while (*cur != '\'' && len) {
		if (*cur == '\0') {
			*cur = ' ';
			overlooking_words_cnt++;
		}
		cur++; len--;
	}

	*cur = '\0';

	return overlooking_words_cnt;
}

static void shift(char **argv, int begin, int shift) {
	for (int i = begin + shift; i < MAX_TOKEN_COUNT; i++) {
		argv[i - shift] = argv[i];
	}
}

static void rebuild_argv(char **argv) {
	int len = MAX_TOKEN_COUNT;
	int i = 0;

	while (NULL != argv[i] && len) {
		if (argv[i][0] == '\'') {
			argv[i]++;
			shift(argv, i + 1, delete_brokens(argv[i]));
		}
		i++;
	}
}

//FIXME now this function return only first graph in list if exist
static struct pnet_graph *get_graph_by_name(char *name) {
	struct pnet_graph *gr;

	list_for_each_entry(gr, &pnet_graphs, lnk) {
		if (0 == strcmp(gr->name, name))
			return gr;
	}

	return NULL;
}

static net_node_t get_node_from_graph_by_name(struct pnet_graph *gr, char *name) {
	net_node_t node;

	list_for_each_entry(node, &gr->nodes, gr_link) {
		if (!strcmp(node->name, name))
			return node;
	}

	return NULL;
}

static void print_graph_names(void) {
	struct pnet_graph *gr;

	printf("%s", "Running graphs: ");
	list_for_each_entry(gr, &pnet_graphs, lnk) {
		if (PNET_GRAPH_STARTED == gr->state)
			printf("%s, ", gr->name);
	}
	printf("\n%s", "Stopped graphs: ");
	list_for_each_entry(gr, &pnet_graphs, lnk) {
		if (PNET_GRAPH_STOPPED == gr->state)
			printf("%s, ", gr->name);
	}
}

static void print_list_of_graph_nodes(struct pnet_graph *gr) {
	net_node_t node = NULL;

	if (list_empty(&gr->nodes)) {
		printf("list of nodes is empty \n");
		return;
	}
	printf("nodes of %s: \n", gr->name);
	list_for_each_entry(node, &gr->nodes, gr_link) {
		printf("	%s (type: %s)\n ", node->name, node->proto->name);
	}
}

static int print_graph(char **argv) {
	net_node_t node;
	struct pnet_graph *gr;
	net_node_t root;

	get_graph(gr, argv[3], ENOENT);

	get_node_from_graph(gr, root, argv[4], ENOENT);

	if (list_empty(&gr->nodes)) {
		printf("list of nodes is empty ");
		return ENOERR;
	}

	node = root->rx_dfault;

	printf("%s (type: %s)\n", root->name, root->proto->name);
	while (NULL != node) {
		printf("%s (type: %s)\n", node->name, node->proto->name);
		node = node->rx_dfault;
	}

	return ENOERR;
}

static void print_rules(net_node_matcher_t node) {
	struct list_head *h;
	match_rule_t rule;
	struct in_addr ip;
	char *dstaddr;
	unsigned char mac[18];
	int counter  = 1;

	list_for_each (h, &node->match_rx_rules) {
		rule = member_cast_out(h, struct match_rule, lnk);

		ip.s_addr = rule->skbuf->nh.iph->saddr;
		dstaddr = inet_ntoa(ip);
		macaddr_print(mac, rule->skbuf->mac.ethh->h_source);

		printf("%d : dst IP - %s, next node - %s, dst HWaddr - %s \n",
				counter, dstaddr, rule->next_node->proto->name, mac);
		counter++;
	}
}

static int add_node(char **argv) {
	struct pnet_graph *gr;
	net_node_t node;
	int graph_state;
	char *name;
	int res;

	get_graph(gr, argv[3], ENOENT);

	if (PNET_GRAPH_STOPPED != (graph_state = gr->state)) {
		pnet_graph_stop(gr);
	}

	get_node_from_repo(node, argv[4], ENOENT);

	if (0 > (res = pnet_graph_add_node(gr, node))) {
		if (res == -EBUSY) {
			printf("node %s is already in use in graph %s", node->name, gr->name);
		}
		return res;
	}

	name  = malloc(strlen(argv[5]));
	strcpy(name, argv[5]);
	node->name = name;

	gr->state = graph_state;

	return ENOERR;
}

/**
 *	@param argv[2] - graph name
 *	@param argv[3] - name of source node
 *	@param argv[4] - name of node will be linked to argv[3]
 */
static int link_node(char **argv) {
	net_node_t src, node;
	struct pnet_graph *gr;
	int graph_state;
	int res;

	get_graph(gr, argv[2], ENOENT);

	if (PNET_GRAPH_STOPPED != (graph_state = gr->state)) {
		pnet_graph_stop(gr);
	}

	get_node_from_graph(gr, src, argv[3], ENOENT);
	get_node_from_graph(gr, node, argv[4], ENOENT);

	res = pnet_node_link(src, node);
	gr->state = graph_state;

	if (res < 0)
		printf("node linking error ");

	return res;
}

static int unlink_node(char **argv) {
	struct pnet_graph *gr;
	net_node_t node;
	int graph_state;

	get_graph(gr, argv[3], ENOENT);

	if (PNET_GRAPH_STOPPED != (graph_state = gr->state)) {
		pnet_graph_stop(gr);
	}

	get_node_from_graph(gr, node, argv[4], ENOENT);

	/* if node->graph == NULL it means that node is not in any graph */
	node->graph = NULL;
	node->rx_dfault = NULL;

	/* delete node from graph */
	list_del(&node->gr_link);

	gr->state = graph_state;

	return ENOERR;
}

static int rule_alloc(char **argv) {
	match_rule_t rule;
	struct pnet_graph *gr;
	net_node_t node;

	get_graph(gr, argv[3], 0);
	get_node_from_graph(gr, node, argv[4], 0);

	if (NULL == (rule = pnet_rule_alloc())) {
		return -ENOMEM;
	}

	pnet_add_new_rx_rule(rule, (net_node_matcher_t) node);

	return ENOERR;
}

static int rule_delete(char **argv) {
	match_rule_t rule;
	struct pnet_graph *gr;
	net_node_t node;

	get_graph(gr, argv[3], 0);
	get_node_from_graph(gr, node, argv[4], 0);
	get_rule_from_node(node, rule, argv[5], 0);

	pnet_remove_rx_rule(rule);
	pnet_rule_free(rule);

	return ENOERR;
}


static int rule_set_mac(struct pnet_graph *gr, match_rule_t rule, char *rule_elem) {
	if (NULL == macaddr_scan((unsigned char *) rule_elem, rule->skbuf->mac.ethh->h_source)) {
		return -EINVAL;
	}
	return ENOERR;
}

static int rule_set_ip(struct pnet_graph *gr, match_rule_t rule, char *rule_elem) {
	if (NULL == ipaddr_scan((unsigned char *) rule_elem, (unsigned char *) &rule->skbuf->nh.iph->saddr)) {
		return -EINVAL;
	}
	return ENOERR;
}

static int rule_set_next_node (struct pnet_graph *gr, match_rule_t rule, char *rule_elem) {
	net_node_t node;

	get_node_from_graph(gr, node, rule_elem, ENOENT);
	pnet_rule_set_next_node(rule, node);

	return ENOERR;
}

/* return rule specified by it position in list of rules */
static match_rule_t rule_get_by_id(net_node_t node, char id) {
	struct list_head *h;
	match_rule_t rule;
	net_node_matcher_t matcher;
	int n;
	int counter = 1;

	sscanf(&id, "%d", &n);

	matcher = (net_node_matcher_t) node;
	list_for_each (h, &matcher->match_rx_rules) {
		rule = member_cast_out(h, struct match_rule, lnk);
		if (counter == n)
			return rule;
		counter++;
	}

	return NULL;
}

static _rule_setter get_setter_by_option(char *option) {
	for (int i = 0; i < RULE_OPTION_QUANTITY; i++) {
		if (!strcmp(rule_setters[i].option, option))
			return rule_setters[i].setter;
	}

	return NULL;
}

static int exec(int argc, char **argv) {
	int opt, res;
	struct pnet_graph *gr;
	const struct pnet_module *mod;
	match_rule_t rule;
	net_node_t node;
	_rule_setter setter;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hgnt:d:p:r:s:a:l:"))) {
		rebuild_argv(argv);

		switch(opt) {
		case 'h':
			print_usage();
			break;
		case 'g':
			print_graph_names();
			break;
		case 'p':
			if (!strcmp("--graph", argv[2])) {
				print_graph(argv);
				break;
			}

			if (!strcmp("--node", argv[2])) {
				get_graph(gr, argv[3], 0);
				print_list_of_graph_nodes(gr);
				break;
			}

			if (!strcmp("--rule", argv[2])) {
				get_graph(gr, argv[3], 0);
				get_node_from_graph(gr, node, argv[4], 0);
				print_rules((net_node_matcher_t) node);
				break;
			}

			printf("%s: no such option ", argv[2]);
			break;
		case 'n' :
			printf("nodes:\n");
			array_foreach_ptr(mod, __pnet_mod_repo, ARRAY_SPREAD_SIZE(__pnet_mod_repo)) {
				if (mod->node)
					printf("	%s\n", mod->node->proto->name);
			}
			printf("\nprotocols:\n");
			array_foreach_ptr(mod, __pnet_mod_repo, ARRAY_SPREAD_SIZE(__pnet_mod_repo)) {
				if (!mod->node)
					printf("	%s\n", mod->proto->name);
			}
			break;
		case 'r':
			get_graph(gr, argv[2], 0);
			pnet_graph_start(gr);
			break;
		case 's':
			get_graph(gr, argv[2], 0);
			pnet_graph_stop(gr);
			break;
		case 'a' :
			if (!strcmp("--graph", argv[2])) {
				if (NULL == pnet_graph_create(argv[3])) {
					printf("%s: graph was not created ", argv[3]);
					return -1;
				}
				break;
			}

			if (!strcmp("--node", argv[2])) {
				if ((res = add_node(argv)) < 0) {
					return 0;
				}
				break;
			}

			if (!strcmp("--rule", argv[2])) {
				if (rule_alloc(argv) < 0) {
					printf("rule was not created ");
					return -1;
				}
				break;
			}

			printf("%s: no such option ", argv[2]);
			return 0;
		case 't':
			get_graph(gr, argv[2], 0);
			get_node_from_graph(gr, node, argv[3], 0);
			get_rule_from_node(node, rule, argv[4], 0);

			if (NULL != (setter = get_setter_by_option(argv[5]))) {
				if (setter(gr, rule, argv[6]) < 0) {
					printf("%s: incorrect value ", argv[6]);
					return 0;
				}
			} else {
				printf("%s: no such option ", argv[5]);
			}
			break;
		case 'l':
			if (link_node(argv) < 0)
				return 0;
			break;
		case 'd' :
			if (!strcmp("--node", argv[2])) {
				if (unlink_node(argv) < 0)
					return 0;
				break;
			}
			if (!strcmp("--rule", argv[2])) {
				if (rule_delete(argv) < 0)
					return 0;
				break;
			}
			printf("%s: no such option ", argv[2]);
			return -1;
		default:
			return 0;
		}
	}

	return 0;
}
