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
#include <stdlib.h>

#include "setup_tty.h"

static const char *script_commands[] = {
	#include <system_start.inc>
};

int system_start(void) {
	const char *command;

	setup_tty(OPTION_STRING_GET(tty_dev));

	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {

		system(command);
	}

	return 0;
}
