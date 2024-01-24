/**
 * @file
 * @brief
 *
 * @date 12.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <lib/libds/dlist.h>
#include <net/netfilter.h>
#include <string.h>

static int clear_rules(int chain) {
	int ret;

	if (chain != NF_CHAIN_UNKNOWN) {
		return nf_clear(chain);
	}

	ret = nf_clear(NF_CHAIN_INPUT);
	if (ret != 0) {
		return ret;
	}

	ret = nf_clear(NF_CHAIN_FORWARD);
	if (ret != 0) {
		return ret;
	}

	ret = nf_clear(NF_CHAIN_OUTPUT);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static void print_header(int chain) {
	printf("Chain %s (policy %s)\n", nf_chain_to_str(chain),
			nf_target_to_str(nf_get_chain_target(chain)));
	printf("target    prot opt  source           destination\n");
}

static void print_rule(const struct nf_rule *r) {
	const char *target_str;
	target_str = nf_target_to_str(r->target);
	printf("%-8s ", target_str != NULL ? target_str : "");
	printf("%c%-4s ", r->not_proto ? '!' : ' ',
			nf_proto_to_str(r->proto));
	printf("%c%-2s ", r->test_hnd ? 'C' : '-', "-");
	printf("%c%-15s ", r->not_saddr && (r->saddr.s_addr
				!= INADDR_ANY) ? '!' : ' ',
			r->saddr.s_addr != INADDR_ANY ? inet_ntoa(r->saddr)
				: "anywhere");
	printf("%c%-15s", r->not_daddr && (r->daddr.s_addr
				!= INADDR_ANY) ? '!' : ' ',
			r->daddr.s_addr != INADDR_ANY ? inet_ntoa(r->daddr)
				: "anywhere");
	if ((r->sport != 0) || (r->dport != 0)) {
		printf(" %s", nf_proto_to_str(r->proto));
		if (r->sport != 0) {
			printf(" spt:");
			if (r->not_sport) {
				printf("!");
			}
			printf("%hu", (unsigned short int)ntohs(r->sport));
		}
		if (r->dport != 0) {
			printf(" dpt:");
			if (r->not_dport) {
				printf("!");
			}
			printf("%hu", (unsigned short int)ntohs(r->dport));
		}
	}
	printf("\n");
}

static void print_rules(int chain) {
	struct dlist_head *rules;
	struct nf_rule *r = NULL;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return;
	}

	print_header(chain);
	dlist_foreach_entry(r, rules, lnk) {
		print_rule(r);
	}
}

static int show_rules(int chain, int rule_num) {
	struct nf_rule *r;

	if (chain != NF_CHAIN_UNKNOWN) {
		if (rule_num == -1) {
			print_rules(chain);
		}
		else {
			r = nf_get_rule_by_num(chain, rule_num);
			if (r == NULL) {
				return -ENOENT;
			}
			print_rule(r);
		}
	}
	else {
		print_rules(NF_CHAIN_INPUT);
		printf("\n");
		print_rules(NF_CHAIN_FORWARD);
		printf("\n");
		print_rules(NF_CHAIN_OUTPUT);
	}

	return 0;
}

int main(int argc, char **argv) {
	int ind, oper, chain, rule_num, not_flag;
	unsigned int port;
	struct nf_rule rule;

	oper = rule_num = -1;
	chain = NF_CHAIN_UNKNOWN;
	not_flag = 0;
	nf_rule_init(&rule);

	for (ind = 1; ind != argc; ++ind) {
		if (!strcmp(argv[ind], "-h")
				|| !strcmp(argv[ind], "--help")) {
			printf("Usage:\n");
			printf("  iptables -A chain rule-specification\n");
			printf("  iptables -I chain [rulenum] rule-specification\n");
			printf("  iptables -R chain rulenum rule-specification\n");
			printf("  iptables -D chain rulenum\n");
			printf("  iptables -F [chain]\n");
			printf("  iptables -L [chain [rulenum]]\n");
			printf("  iptables -P chain target\n");
			return 0;
		}
		else if (oper == -1) {
			if ((0 == strcmp(argv[ind], "-A"))
					|| (0 == strcmp(argv[ind], "--append"))) {
				oper = 'A';
			}
			else if ((0 == strcmp(argv[ind], "-I"))
					|| (0 == strcmp(argv[ind], "--insert"))) {
				oper = 'I';
			}
			else if ((0 == strcmp(argv[ind], "-R"))
					|| (0 == strcmp(argv[ind], "--replace"))) {
				oper = 'R';
			}
			else if ((0 == strcmp(argv[ind], "-D"))
					|| (0 == strcmp(argv[ind], "--delete"))) {
				oper = 'D';
			}
			else if ((0 == strcmp(argv[ind], "-F"))
					|| (0 == strcmp(argv[ind], "--flush"))) {
				oper = 'F';
			}
			else if ((0 == strcmp(argv[ind], "-L"))
					|| (0 == strcmp(argv[ind], "--list"))) {
				oper = 'L';
			}
			else if ((0 == strcmp(argv[ind], "-P"))
					|| (0 == strcmp(argv[ind], "--policy"))) {
				oper = 'P';
			}
			else {
				printf("iptables: unknown command: '%s`\n", argv[ind]);
				return -EINVAL;
			}
		}
		else if (chain == NF_CHAIN_UNKNOWN) {
			chain = nf_chain_get_by_name(argv[ind]);
			if (chain == NF_CHAIN_UNKNOWN) {
				if ((oper == 'A') || (oper == 'I') || (oper == 'R')
						|| (oper == 'D') || (oper == 'P')) {
					printf("iptables: unknown chain: `%s'\n",
							argv[ind]);
					return -EINVAL;
				}
				else {
					/* it's not chain */
					goto after_chain;
				}
			}
		}
		else
after_chain:
		/* else */ if (((oper == 'R') || (oper == 'D') || (oper == 'I')
					|| (oper == 'L'))
				&& (rule_num == -1)) {
			if ((1 != sscanf(argv[ind], "%d", &rule_num))
					|| (rule_num <= 0)) {
				if ((oper == 'R') || (oper == 'D')) {
					printf("iptables: invalid rule_num: `%s'\n",
							argv[ind]);
					return -EINVAL;
				}
				else {
					/* it's not rule_num */
					goto after_rule_num;
				}
			}
			rule_num -= 1;
		}
		else
after_rule_num:
		/* else */ if ((oper == 'P')
				&& (rule.target == NF_TARGET_UNKNOWN)) {
			rule.target = nf_target_get_by_name(argv[ind]);
			if (rule.target == NF_TARGET_UNKNOWN) {
				printf("iptables: unknown target: `%s'\n", argv[ind]);
				return -EINVAL;
			}
		}
		else if ((oper != 'A') && (oper != 'I') && (oper != 'R')) {
			printf("iptables: invalid option: `%s'\n", argv[ind]);
			return -EINVAL;
		}
		else if ((0 == strcmp(argv[ind], "-j"))
				|| (0 == strcmp(argv[ind], "--jump"))) {
			if (++ind == argc) {
				printf("iptables: no target specified\n");
				return -EINVAL;
			}
			rule.target = nf_target_get_by_name(argv[ind]);
			if (rule.target == NF_TARGET_UNKNOWN) {
				printf("iptables: unknown target: `%s'\n", argv[ind]);
				return -EINVAL;
			}
		}
		else if (0 == strcmp(argv[ind], "!")) {
			not_flag = 1;
			continue;
		}
		else if ((0 == strcmp(argv[ind], "-p"))
				|| (0 == strcmp(argv[ind], "--protocol"))) {
			if (++ind == argc) {
				printf("iptables: no protocol specified\n");
				return -EINVAL;
			}
			rule.proto = nf_proto_get_by_name(argv[ind]);
			if ((rule.proto == NF_PROTO_ALL) && not_flag) {
				printf("iptables: rule would never match protocol\n");
				return -EINVAL;
			}
			else if (rule.proto == NF_PROTO_UNKNOWN) {
				printf("iptables: invalid protocol: `%s'\n", argv[ind]);
				return -EINVAL;
			}
			NF_SET_NOT_FIELD(&rule, proto, not_flag, rule.proto);
			not_flag = 0;
		}
		else if ((0 == strcmp(argv[ind], "-s"))
				|| (0 == strcmp(argv[ind], "--source"))) {
			if (++ind == argc) {
				printf("iptables: no source address specified\n");
				return -EINVAL;
			}
			if (0 == inet_aton(argv[ind], &rule.saddr)) {
				printf("iptables: invalid source address: `%s'\n", argv[ind]);
				return -EINVAL;
			}
			NF_SET_NOT_FIELD(&rule, saddr, not_flag, rule.saddr);
			not_flag = 0;
		}
		else if ((0 == strcmp(argv[ind], "-d"))
				|| (0 == strcmp(argv[ind], "--destination"))) {
			if (++ind == argc) {
				printf("iptables: no destination address specified\n");
				return -EINVAL;
			}
			if (0 == inet_aton(argv[ind], &rule.daddr)) {
				printf("iptables: invalid destination address: `%s'\n", argv[ind]);
				return -EINVAL;
			}
			NF_SET_NOT_FIELD(&rule, daddr, not_flag, rule.daddr);
			not_flag = 0;
		}
		else if (rule.set_proto
				&& ((rule.proto == NF_PROTO_TCP)
					|| (rule.proto == NF_PROTO_UDP))
				&& !rule.not_proto
				&& ((0 == strcmp(argv[ind], "--sport"))
					|| (0 == strcmp(argv[ind], "--source-port")))) {
			if (++ind == argc) {
				printf("iptables: no source port address specified\n");
				return -EINVAL;
			}
			if (1 != sscanf(argv[ind], "%u", &port)) {
				printf("iptables: invalid source port: `%s'\n", argv[ind]);
				return -EINVAL;
			}
			NF_SET_NOT_FIELD(&rule, sport, not_flag,
					htons((unsigned short)port));
			not_flag = 0;
		}
		else if (rule.set_proto
				&& ((rule.proto == NF_PROTO_TCP)
					|| (rule.proto == NF_PROTO_UDP))
				&& !rule.not_proto
				&& ((0 == strcmp(argv[ind], "--dport"))
					|| (0 == strcmp(argv[ind], "--destination-port")))) {
			if (++ind == argc) {
				printf("iptables: no destination port address specified\n");
				return -EINVAL;
			}
			if (1 != sscanf(argv[ind], "%u", &port)) {
				printf("iptables: invalid destination port: `%s'\n", argv[ind]);
				return -EINVAL;
			}
			NF_SET_NOT_FIELD(&rule, dport, not_flag,
					htons((unsigned short)port));
			not_flag = 0;
		}
		else {
			printf("iptables: unknown option: `%s'\n", argv[ind]);
			return -EINVAL;
		}
		if (not_flag) {
			printf("iptables: unexpected ! flag\n");
			return -EINVAL;
		}
	}

	if (oper == -1) {
		printf("iptables: no command specified\n");
		return -EINVAL;
	}

	if (((oper == 'A') || (oper == 'I') || (oper == 'R') || (oper == 'D')
				|| (oper == 'P'))
			&& (chain == NF_CHAIN_UNKNOWN)) {
		printf("iptables: no chain specified\n");
		return -EINVAL;
	}

	if (rule_num == -1) {
		if ((oper == 'R') || (oper == 'D')) {
			printf("iptables: no rule_num specified\n");
			return -EINVAL;
		} else if (oper == 'I') {
			rule_num = 0;
		}
	}

	if ((oper == 'P') && (rule.target == NF_TARGET_UNKNOWN)) {
		printf("iptables: no target specified\n");
		return -EINVAL;
	}

	return oper == 'A' ? nf_add_rule(chain, &rule)
			: oper == 'I' ? nf_insert_rule(chain, &rule, rule_num)
			: oper == 'R' ? nf_set_rule(chain, &rule, rule_num)
			: oper == 'D' ? nf_del_rule(chain, rule_num)
			: oper == 'F' ? clear_rules(chain)
			: oper == 'P' ? nf_set_chain_target(chain, rule.target)
			: /* oper == 'L' ? */ show_rules(chain, rule_num);
}
