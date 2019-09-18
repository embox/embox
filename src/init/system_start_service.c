/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */
#include <util/log.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/array.h>

#include <framework/cmd/api.h>
#include <cmd/shell.h>
#include <cmd/cmdline.h>

#include "setup_tty.h"

static const char *script_commands[] = {
	#include <system_start.inc>
};

int system_start(void) {
	const char *command;
	char *argv[10];
	int argc;
	const struct cmd *cmd;
	char cmd_line[64];
	const char *tty_dev_name;

	tty_dev_name = setup_tty(OPTION_STRING_GET(tty_dev));

	printf("Default IO device[%s]\n", tty_dev_name);

	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		strncpy(cmd_line, command, sizeof(cmd_line) - 1);
		cmd_line[sizeof(cmd_line) - 1] = '\0';
#if OPTION_GET(NUMBER,log_level) >= LOG_INFO
		printf(">%s\n", command);
#endif
		argc = cmdline_tokenize((char *)cmd_line, argv);
		if (0 == strncmp(argv[0], "pthread", 7)) {
			cmd = cmd_lookup(argv[1]);
			continue;
		}
		cmd = cmd_lookup(argv[0]);
		cmd_exec(cmd, argc, argv);
	}

	return 0;
}
