/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <errno.h>
#include <sys/resource.h>
#include <stdio.h>
#include <framework/cmd/api.h>
#include <string.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int incr, ind, prior;
	const struct cmd *cmd;

	prior = getpriority(PRIO_PROCESS, 0);
	if (prior == -1) {
		return -errno;
	}

	if (argc == 1) {
		printf("%d\n", prior);
		return 0;
	}

	prior = 10;
	for (ind = 1; ind < argc; ++ind) {
		if (strcmp(argv[ind], "-h") == 0) {
			printf("Usage: %s [-n increment] utility [argument...]\n", argv[0]);
			return 0;
		}
		else if (strcmp(argv[ind], "-n") == 0) {
			if ((++ind == argc) || (sscanf(argv[ind], "%d", &incr) != 1)) {
				return -EINVAL;
			}
			prior += incr;
		}
		else {
			break;
		}
	}

	if (ind >= argc) {
		return -EINVAL;
	}

	cmd = cmd_lookup(argv[ind]);
	if (cmd == NULL) {
		printf("%s: %s: Command not found\n", argv[0], argv[ind]);
		return -ENOENT;
	}

	if (-1 == setpriority(PRIO_PROCESS, 0, prior)) {
		return -errno;
	}

	return cmd_exec(cmd, argc - ind, argv + ind);
}
