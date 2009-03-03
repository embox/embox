/*
 * help.c
 *
 *  Created on: 02.03.2009
 *      Author: abatyukov
 */
#include "types.h"
#include "conio.h"
#include "shell.h"
#include "help.h"

static char help_keys[] = {
#include "help_keys.inc"
		};

static void show_help() {
	printf(
#include "help_help.inc"
	);
}

int help_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev;
	int i;

	keys_amount = parse_arg("help", argsc, argsv, help_keys, sizeof(help_keys),
			keys);

//	 old:
//		if (keys_amount < 1) {
//
//	 on "help" without arguments show all commands
//	 old version never shows commands!
//	 .Fomka

	if (keys_amount < 0) {
		// Error state:
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	//need to display all possible commands
	printf("Available commands: \n");
	for (i = 0; i < sizeof(shell_handlers) / sizeof(shell_handlers[0]); i++) {
//		old:
//		printf("%s\t\t%s\n", shell_handlers[i].name, shell_handlers[i].description);
//		 Put " - " in help, not in every single description
//		 .Fomka
		printf("%s\t\t - %s\n", shell_handlers[i].name, shell_handlers[i].description);
	}

	return 0;
}
