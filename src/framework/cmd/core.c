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

// TODO cmdline_xxx aren't part of cmd framework,
// move them to some utils library. -- Eldar
static char *cmdline_next_token(const char **str) {
	char *ret;
	const unsigned char *ptr = (const unsigned char *) *str;

	/* Skip whitespace characters. */
	while (isspace(*ptr)) {
		++ptr;
	}

	if (!*ptr) {
		/* Got EOL. */
		return NULL;
	}

	/* Found start of token. */
	ret = (char *) ptr;

	/* Now skip all non-whitespace characters to get end of token. */
	while (*ptr && !isspace(*ptr)) {
		++ptr;
	}

	/* Save end of token into the argument. */
	*str = (const char *) ptr;
	return ret;
}

int cmdline_tokenize(char *cmdline, char **argv) {
	int argc = 0;
	char *token;

	while ((token = cmdline_next_token((const char **) &cmdline))) {
		argv[argc++] = token;
		if (*cmdline) {
			*cmdline++ = '\0';
		}
	}

	return argc;
}
