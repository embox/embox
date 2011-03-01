/**
 * @file
 * @brief TODO
 *
 * @date Mar 1, 2011
 * @author Eldar Abusalimov
 */

#include <cmd/framework.h>

#include <stddef.h>
#include <errno.h>

#include <util/array.h>
#include <mod/core.h>

ARRAY_SPREAD_DEF(const struct cmd, __cmd_registry);

int cmd_exec(const struct cmd *cmd, int argc, char **argv) {
	if (NULL == cmd) {
		return -EINVAL;
	}
	if (NULL == cmd->exec) {
		return -EBADF;
	}

	return cmd->exec(argc, argv);
}

