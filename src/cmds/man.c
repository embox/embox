/**
 * @file
 * @brief display the on-line manual pages
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <util/array.h>
#include <framework/cmd/api.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	const struct cmd *cmd;
	char *name;

	if (argc <= 1) {
		printf("What manual page do you want?\n");
		return 0;
	}

	array_foreach(name, argv + 1, argc - 1) {
		if (NULL != (cmd = cmd_lookup(name))) {
			printf("Manual entry for %s:\n%s\n\n", name, cmd_details(cmd));
		} else {
			printf("No manual entry for %s\n\n", name);
		}
	}

	return 0;
}
