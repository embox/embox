/**
 * @file
 * @brief
 *
 * @date 28.02.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>

EMBOX_CMD(exec);

static int exec(int argc, char *argv[]) {
	char **env;

	if (!environ) return 0;

	env = environ;
	do
		printf("%s\n", *env);
	while (*++env);

	return 0;
}
