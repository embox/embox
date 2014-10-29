/**
 * @file
 *
 * @date Mar 29, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <cmd/shell.h>

#include <stdlib.h>

int system(const char *command) {
	const struct shell *sh = shell_any();

	if (!sh) {
		return -ENOENT;
	}

	return shell_exec(sh, command);
}
