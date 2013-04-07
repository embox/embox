/**
 * @file
 * @brief Command registry and invocation code.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#include <framework/cmd/api.h>

#include <ctype.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#include <util/array.h>

ARRAY_SPREAD_DEF(const struct cmd, __cmd_registry);

int cmd_exec(const struct cmd *cmd, int argc, char **argv) {
	if (NULL == cmd) {
		return -EINVAL;
	}

	return cmd->exec(argc, argv);
}

const struct cmd *cmd_lookup(const char *name) {
	const struct cmd *cmd = NULL;

	cmd_foreach(cmd) {
		if (strcmp(cmd_name(cmd), name) == 0) {
			return cmd;
		}
	}

	return NULL;
}
