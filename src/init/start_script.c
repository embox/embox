/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <util/array.h>
#include <embox/unit.h>
#include <framework/cmd/api.h>
#include <cmd/shell.h>
#include <stdio.h>

#include "setup_tty.h"

#define START_SHELL OPTION_GET(NUMBER,shell_start)

EMBOX_UNIT_INIT(run_script);

static const char *script_commands[] = {
	#include <start_script.inc>
};

static int run_script(void) {
	const char *command;
	const struct shell *shell;

	shell = shell_lookup(OPTION_STRING_GET(shell_name));
	if (NULL == shell) {
		shell = shell_any();
		if (NULL == shell) {
			return -ENOENT;
		}
	}

	setup_tty(OPTION_STRING_GET(tty_dev));

	printf("\nStarted shell [%s] on device [%s]\n",
		OPTION_STRING_GET(shell_name), OPTION_STRING_GET(tty_dev));

	printf("loading start script:\n");
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		int ret;

		printf("> %s \n", command);

		ret = shell_exec(shell, command);

		if (OPTION_GET(BOOLEAN,stop_on_error) && ret) {
			return ret;
		}
	}

#if START_SHELL
	shell_run(shell);
#endif

	return 0;
}
