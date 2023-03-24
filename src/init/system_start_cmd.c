/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <util/log.h>
#include <framework/cmd/api.h>
#include <cmd/cmdline.h>

#define CMD_MAX_ARGV	10

int system_start(void) {
	char *argv[CMD_MAX_ARGV];
	int argc;
	const struct cmd *cmd;

	argc = cmdline_tokenize(OPTION_STRING_GET(cmd), argv);

	cmd = cmd_lookup(argv[0]);
	if (cmd == NULL) {
		log_error("cmd %s did not find\n", argv[0]);
		return -1;
	}

	return cmd_exec(cmd, argc, argv);
}
