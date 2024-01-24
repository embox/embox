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

#include <lib/libds/array.h>

#include <framework/cmd/api.h>

#include <cmd/cmdline.h>

#include "setup_tty.h"

#define STOP_ON_ERROR OPTION_GET(BOOLEAN,stop_on_error)

#define CMD_MAX_ARGV OPTION_GET(NUMBER, cmd_max_argv)
#define CMD_MAX_LEN  OPTION_GET(NUMBER, cmd_max_len)

static const char *script_commands[] = {
	#include <system_start.inc>
};

extern int graphic_init(void);

int system_start(void) {
	const char *command;
	char *argv[CMD_MAX_ARGV];
	int argc;
	const struct cmd *cmd;
	char cmd_line[CMD_MAX_LEN];
	const char *tty_dev_name;
	int ret;

	tty_dev_name = setup_tty(OPTION_STRING_GET(tty_dev));

	printf("Default IO device[%s]\n", tty_dev_name);

	graphic_init();

	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		strncpy(cmd_line, command, sizeof(cmd_line) - 1);
		cmd_line[sizeof(cmd_line) - 1] = '\0';
#if OPTION_GET(STRING, log_level) >= LOG_INFO
		printf(">%s\n", cmd_line);
#endif
		argc = cmdline_tokenize((char *)cmd_line, argv);
		if (0 == strncmp(argv[0], "pthread", 7)) {
			cmd = cmd_lookup(argv[1]);
			continue;
		}
		cmd = cmd_lookup(argv[0]);
		if (cmd == NULL) {
			printf("cmd %s did not find\n", argv[0]);
#if STOP_ON_ERROR
			return -ENOENT;
#endif
		}
		ret = cmd_exec(cmd, argc, argv);
		if (ret) {
			printf("cmd %s failed with error (%d)\n", argv[0], ret);
#if STOP_ON_ERROR
			return ret;
#endif
		}
	}

	return 0;
}
