/**
 * @file
 *
 * @date Oct 4, 2012
 * @author: Anton Bondarev
 */
#include <cmd/shell.h>
#include <string.h>


ARRAY_SPREAD_DEF(const struct shell, __shell_registry);

const struct shell *shell_lookup(const char *shell_name) {
	const struct shell *shell;

	if (!strncmp(shell_name, "/bin/", strlen("/bin/"))) {
		shell_name += strlen("/bin/");
	}

	array_spread_foreach_ptr(shell, __shell_registry) {
		if (0 == strcmp(shell->name, shell_name)) {
			return shell;
		}
	}

	return NULL;
}

const struct shell *shell_any(void) {
	const struct shell *shell;

	array_spread_foreach_ptr(shell, __shell_registry) {
		if (shell) {
			return shell;
		}
	}

	return NULL;
}
