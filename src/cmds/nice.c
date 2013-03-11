/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <framework/cmd/api.h>

EMBOX_CMD(exec);

static int niceness = 0;

static int exec(int argc, char **argv) {
	int opt, n, i;
	const struct cmd *cmd;

	if (argc == 1) {
		printf("%d\n", niceness);
		return 0;
	}

	i = 1;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hn:"))
			&& opt != '?') {
		switch (opt) {
		default:
			return -EINVAL;
		case 'h':
			printf("Usage: nice [-n <N>] <command> [args]\n");
			return 0;
		case 'n':
			if (sscanf(optarg, "%d", &n) != 1) {
				return -EINVAL;
			}
			i += 2;
			break;
		}
	}

	if (i >= argc) {
		return -EINVAL;
	}

	cmd = cmd_lookup(argv[i]);
	if (cmd == NULL) {
		printf("%s: %s: Command not found\n", argv[0], argv[i]);
		return -ENOENT;
	}

	if (nice(n + niceness) == -1) {
		return -errno;
	}

	return cmd_exec(cmd, argc - i, argv + i);
}
