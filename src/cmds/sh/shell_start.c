/**
 * @file
 * @brief Shell starter
 * @details As a shell_api doesn't provide autostart on load, this
 * module starts shell on load
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#include <stdio.h>
#include <cmd/shell.h>
#include <embox/unit.h>
#include <util/array.h>

EMBOX_UNIT_INIT(shell_start);

static const char *script_commands[] = {
	#include <start_script.inc>
};

static int shell_start(void) {
	const char *command;
	char *line;
	printf("\nloading start script\n");
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		printf("> %s \n", command);
		*line = *command;
		shell_line_input(line);
	}
	shell_run();
	return 0;
}
