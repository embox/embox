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
    const char tish_cmd[] = "tish";

	if (!strncmp(shell_name, "/bin/", strlen("/bin/"))) {
		shell_name += strlen("/bin/");
	}

    /* Make a hardlink /bin/sh to tish, until a more solid solution*/
    if (strcmp(shell_name, "sh") == 0) {
        array_spread_foreach_ptr(shell, __shell_registry) {
            if (0 == strcmp(shell->name, tish_cmd)) {
                return shell;
            }
        }
    }
    else {
        array_spread_foreach_ptr(shell, __shell_registry) {
            if (0 == strcmp(shell->name, shell_name)) {
                return shell;
            }
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
