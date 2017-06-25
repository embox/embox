/**
 * @file
 * @brief Command registry and invocation code.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#include <framework/cmd/api.h>
#include <framework/cmd/types.h>

#include <ctype.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#include <util/array.h>

ARRAY_SPREAD_DEF(const struct cmd * const, __cmd_registry);

extern void getopt_init(void);

int cmd_exec(const struct cmd *cmd, int argc, char **argv) {
	int err;

	if (!cmd)
		return -EINVAL;

	err = mod_activate_app(cmd2mod(cmd));
	if (err)
		return err;

	getopt_init();

	return cmd->exec(argc, argv);
}

const struct cmd *cmd_lookup(const char *name) {
	const struct cmd *cmd = NULL;

	if (!strncmp(name, "/bin/", strlen("/bin/"))) {
		name += strlen("/bin/");
	}

	cmd_foreach(cmd) {
		if (strcmp(cmd_name(cmd), name) == 0) {
			return cmd;
		}
	}

	return NULL;
}

