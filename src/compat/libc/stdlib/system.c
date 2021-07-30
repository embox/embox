/**
 * @file
 *
 * @date Mar 29, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stdlib.h>

#include <cmd/cmdline.h>
#include <framework/cmd/api.h>

int system(const char *command) {
	int argc;
	char *argv[32];
	const struct cmd *cmd;

	argc = cmdline_tokenize((char *)command, argv);

	cmd = cmd_lookup(argv[0]);
	return cmd_exec(cmd, argc, argv);
}
