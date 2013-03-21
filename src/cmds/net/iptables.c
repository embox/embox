/**
 * @file
 * @brief
 *
 * @date 12.03.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <util/list.h>
#include <net/netfilter.h>
#include <string.h>

EMBOX_CMD(exec);

static int show_rules(void) {
	struct nf_rule *r;

	printf("Chain INPUT\n");
	printf("target    prot source          destination\n");
	list_foreach(r, &nf_input_rules, lnk) {
		printf("%-6s    all  %-15s anywhere\n",
				r->policy == NF_POLICY_DROP ? "DROP" : "ACCEPT",
				inet_ntoa(r->addr.in));
	}
	printf("\n");

	printf("Chain FORWARD\n");
	printf("target    prot source          destination\n");
	list_foreach(r, &nf_forward_rules, lnk) {
		printf("%-6s    all  %-15s anywhere\n",
				r->policy == NF_POLICY_DROP ? "DROP" : "ACCEPT",
				inet_ntoa(r->addr.in));
	}
	printf("\n");

	printf("Chain OUTPUT\n");
	printf("target    prot source          destination\n");
	list_foreach(r, &nf_output_rules, lnk) {
		printf("%-6s    all  %-15s anywhere\n",
				r->policy == NF_POLICY_DROP ? "DROP" : "ACCEPT",
				inet_ntoa(r->addr.in));
	}

	return 0;
}

static void print_usage(void) {
	printf("Usage:\n");
	printf("  iptables {-A|-R|-D} chain options\n");
	printf("  iptables {-F|-L}\n");
	printf("  iptables -h\n");
}

static int exec(int argc, char **argv) {
	int opt, oper, chain;
	struct nf_rule rule;

	oper = -1;
	chain = -1;
	rule.policy = -1;
	rule.not_addr = 1;
	rule.addr.in.s_addr = 0;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "A:D:R:FLs:j:h"))) {
		switch (opt) {
		default:
		case '?':
			printf("iptables: invalid option: `-%c'\n", optopt);
			return -EINVAL;
		case 'F':
			return nf_clear();
		case 'L':
			return show_rules();
		case 'A':
		case 'R':
		case 'D':
			oper = opt;
			if (strcmp("INPUT", optarg) == 0) {
				chain = NF_CHAIN_INPUT;
			}
			else if (strcmp("FORWARD", optarg) == 0) {
				chain = NF_CHAIN_FORWARD;
			}
			else if (strcmp("OUTPUT", optarg) == 0) {
				chain = NF_CHAIN_OUTPUT;
			}
			else {
				printf("iptables: unknown chain: `%s'\n", optarg);
				return -EINVAL;
			}
			break;
		case 's':
			if (inet_aton(optarg, &rule.addr.in) == 0) {
				printf("iptables: invalid address: `%s'\n", optarg);
				return -EINVAL;
			}
			rule.not_addr = 0;
			break;
		case 'j':
			if (strcmp("DROP", optarg) == 0) {
				rule.policy = NF_POLICY_DROP;
			}
			else if (strcmp("ACCEPT", optarg) == 0) {
				rule.policy = NF_POLICY_ACCEPT;
			}
			else {
				printf("iptables: unknown policy: `%s'\n", optarg);
				return -EINVAL;
			}
			break;
		case 'h':
			print_usage();
			return 0;
		}
	}

	if (oper == -1) {
		printf("iptables: no command specified\n");
		return -EINVAL;
	}

	if (chain == -1) {
		printf("iptables: chain not specified\n");
		return -EINVAL;
	}

	if (rule.policy == -1) {
		printf("iptables: target not specified\n");
		return -EINVAL;
	}

	return oper == 'A' ? nf_add_rule(chain, &rule, 0)
			: oper == 'R' ? nf_add_rule(chain, &rule, 1)
			: oper == 'D' ? nf_del_rule(chain, &rule)
			: -EINVAL;
}
