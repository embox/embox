/*
 * help.c
 *
 *  Created on: 02.03.2009
 *      Author: abatyukov
 */
#include "shell.h"
#include "types.h"
#include "common.h"

#include "shell.h"


char help_keys[] = {
#include "help_keys.inc"
};

int help_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev;
	int i;

	keys_amount = parse_arg("help", argsc, argsv, help_keys, sizeof(help_keys),
				keys);

	if (keys_amount < 0) {
		printf(
#include "help_help.inc"
		);
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
			printf(
	#include "help_help.inc"
			);
			return 0;
		}

	//need to display all possible commands
	printf("Available commands are: \n");
	for (i = 0; i < sizeof(shell_handlers) / sizeof(shell_handlers[0]); i++) {
		printf("\t%10s\t%60s\n", shell_handlers[i].name, shell_handlers[i].description);
		}

	return 0;
}
