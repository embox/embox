/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <framework/cmd/api.h>
#include <kernel/task.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int ret, ind, old_prior, prior;
	const struct cmd *cmd;
	char backup_task_name[32];

	errno = 0;
	old_prior = getpriority(PRIO_PROCESS, 0);
	if ((old_prior == -1) && (errno != 0)) {
		return -errno;
	}

	if (argc == 1) {
		printf("%d\n", old_prior);
		return 0;
	}

	prior = 10;
	for (ind = 1; ind < argc; ++ind) {
		if (strcmp(argv[ind], "-h") == 0) {
			printf("Usage: %s [-n increment] utility [argument...]\n", argv[0]);
			return 0;
		}
		else if (strcmp(argv[ind], "-n") == 0) {
			if ((++ind == argc) || (sscanf(argv[ind], "%d", &prior) != 1)) {
				return -EINVAL;
			}
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

	if (-1 == setpriority(PRIO_PROCESS, 0, old_prior + prior)) {
		return -errno;
	}

	strlcpy(backup_task_name, task_get_name(task_self()), sizeof(backup_task_name));
	task_set_name(task_self(), argv[ind]);

	ret = cmd_exec(cmd, argc - ind, argv + ind); /* TODO create new task */

	task_set_name(task_self(), backup_task_name);

	setpriority(PRIO_PROCESS, 0, old_prior); /* restore old priority */

	return ret;
}
