/*
 * help.c
 *
 *  Created on: 02.03.2009
 *      Author: abatyukov
 */
#include "conio.h"
#include "shell.h"
#include "help.h"
#include "shell_command.h"


#define COMMAND_NAME "help"
#define COMMAND_DESC_MSG "show all available command"
static const char *help_msg =
	#include "help_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG)

static char help_keys[] = {
#include "help_keys.inc"
		};

static void show_help() {
	printf(
#include "help_help.inc"
	);
}


static int exec(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev;
	int i;
	SHELL_HANDLER_DESCR *shell_handlers;

	keys_amount = parse_arg("help", argsc, argsv, help_keys, sizeof(help_keys),
			keys);


	if (keys_amount < 0) {
		// Error state:
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

//	if (NULL == (shell_handlers = shell_get_command_list())) {
//		LOG_ERROR("can't find command list\n");
//		return -1;
//	}
//	//need to display all possible commands
//	printf("Available commands: \n");
//	for (i = 0; i < shell_size_command_list(); i++) {
//		printf("%s\t\t - %s\n", shell_handlers[i].name, shell_handlers[i].description);
//	}

	printf("Available commands: \n");
	SHELL_COMMAND_DESCRIPTOR * shell_desc;
	for(shell_desc = shell_command_iterate_start((char *)NULL, 0); NULL != shell_desc; shell_command_iterate_next(shell_desc, (char *)NULL, 0)){
		printf("%s\t\t - %s\n", shell_desc->name, shell_desc->description);
	}
	return 0;
}




