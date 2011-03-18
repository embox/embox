/**
 * @file TODO delete after terminal debugging
 *
 * @brief Temporary cmd for terminal debugging
 *
 * @date 13.11.10
 * @author Anton Bondarev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <lib/readline.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: version [-h]\n");
}

static int exec(int argsc, char **argsv) {
	char *line;

	line = readline(CONFIG_SHELL_PROMPT);

	printf("received '%s'\n", line);
	freeline(line);

	return 0;
}
