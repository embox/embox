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
	const struct cmd *cmd;
	char *argv[0x20];
	int code;
	int argc;

	if (0 == (argc = cmdline_tokenize((char *)command, argv))) {
		return -EINVAL;
	}

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
		//printf("%s: Command not found\n", argv[0]);
		return -ENOENT;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		//printf("%s: Command returned with code %d: %s\n", cmd_name(cmd), code, strerror(-code));
	}
	return code;
}
