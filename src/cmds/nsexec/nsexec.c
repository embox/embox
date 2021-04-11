#include <stdio.h>
#include <net/net_namespace.h>
#include <framework/cmd/api.h>

int main(int argc, char **argv) {
	char *ns = argv[1];
	const struct cmd *cmd;

	if (!(setns_by_name(ns) == 0 || unshare_by_name(ns) == 0))
		return -1;

	if (argc > 2) {
		cmd = cmd_lookup(argv[2]);
		argc -= 2;
		argv += 2;
		cmd_exec(cmd, argc, argv);
	}
	return 0;
}
