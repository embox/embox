/**
 * @file
 * @brief Command registry and invocation code.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#include <framework/cmd/api.h>
#include <framework/cmd/types.h>
#include <framework/mod/options.h>

#include <ctype.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#include <lib/libds/array.h>
#include <util/getopt.h>

ARRAY_SPREAD_DEF(const struct cmd * const, __cmd_registry);

int cmd_exec(const struct cmd *cmd, int argc, char **argv) {
	int err;

	if (!cmd)
		return -EINVAL;

	err = mod_activate_app(cmd2mod(cmd));
	if (err)
		return err;

	getopt_init();

	err = cmd->exec(argc, argv);

	/* FIXME Here we make app's data and bss as they was
	 * before app execution. It's required because we call all
	 * C++ ctors on every app launch. When we will call only ctors
	 * of the running app, this workaround can be removed. */
	mod_activate_app(cmd2mod(cmd));

	return err;
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

