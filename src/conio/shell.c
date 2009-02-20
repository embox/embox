/*
 * shell.c
 *
 *  Created on: 02.02.2009
 *      Author: Alexey Fomin
 */

#include "shell.h"
#include "string.h"
#include "tty.h"
#include "mem.h"
#include "wmem.h"
#include "lspnp.h"

static const char* welcome = "monitor> ";

int stub_shell_handler(int argsc, char **argsv) {
	int i;
	if (argsc == 0) {
		printf("Stub! Handler started with no arguments.\n");
		return 0;
	}
	printf("Stub! Handler started with arguments: ");
	for (i = 0; i < argsc; i++) {
		printf("%s%c ", argsv[i], i + 1 == argsc ? '.' : ',');
	}
	printf("\n");
	return 0;
}

int stub_shell_handler2(int argsc, char **argsv) {
	int i;
	if (argsc == 0) {
		printf("Stub2! Handler started with no arguments.\n");
		return 0;
	}
	printf("Stub2! Handler started with arguments: ");
	for (i = 0; i < argsc; i++) {
		printf("%s%c ", argsv[i], i + 1 == argsc ? '.' : ',');
	}
	printf("\n");
	return 0;
}

typedef struct {
	char *name;
	PSHELL_HANDLER phandler;
} SHELL_HANDLER_DESCR;

static SHELL_HANDLER_DESCR shell_handlers[] = {
#include "shell.inc"
		};

// *str becomes pointer to first non-space character
static void skip_spaces(char **str) {
	while (**str == ' ') {
		(*str)++;
	}
}

// *str becomes pointer to first space or '\0' character
static void skip_word(char **str) {
	while (**str != '\0' && **str != ' ') {
		(*str)++;
	}
}

static int parse_str(char *cmdline, char **words) {
	int cnt = 0;
	while (*cmdline != '\0') {
		if (' ' == *cmdline) {
			*cmdline++ = '\0';
			skip_spaces(&cmdline);
		} else {
			words[cnt++] = cmdline;
			skip_word(&cmdline);
		}
	}
	return cnt;
}

static void tty_callback(char cmdline[]) {
	int words_counter = 0;
	int i;
	PSHELL_HANDLER phandler;

	// alloc at least 1 array element
	char *words[1 + strlen(cmdline) / 2];

	if (0 == (words_counter = parse_str(cmdline, words))) {
		// Only spaces were entered
		return;
	}

	// choosing correct handler
	// XXX  / sizeof(shell_handlers[0]) !
	// (Eldar)
	for (i = 0; i < sizeof(shell_handlers) / sizeof(shell_handlers[0]); i++) {
		if (strcmp(words[0], shell_handlers[i].name)) {
			phandler = shell_handlers[i].phandler;
			phandler(words_counter - 1, words + 1);
			return;
		}
	}
	// handler not found:
	printf("%s: Command not found\n", words[0]);

}

//int process_create()
//{
//	save_context();
//	phandler(handler_args_counter, handler_args);
//
//	restore_context()
//}

int shell_find_commands(char *cmdline, char **proposals) {
	int i, commands_found = 0;
	char *searching_command;

	// Skipping first spaces
	skip_spaces(&cmdline);

	for (i = 0; i < sizeof(shell_handlers); i++) {
		if (sz_cmp_beginning(searching_command, shell_handlers[i].name)) {
			proposals[commands_found++] = shell_handlers[i].name;
		}
	}
	return commands_found;
}

void shell_start() {
	tty_start(tty_callback, shell_find_commands, welcome);
}

// parse arguments array on keys-value structures
// RETURNS:
// -1 wrong condition found. Arguments not in format: -<key> [<value>]
// -2 too many keys entered
// -3 wrong key entered
// amount of entered keys otherwise (if everything's OK)
int parse_arg(const char *handler_name, int argsc, char **argsv,
		char *available_keys, int amount_of_available_keys, SHELL_KEY *keys) {
	int i, j, args_count;

	i = 0;
	args_count = 0;

	while (i < argsc) {
		if (*argsv[i] != '-') {
			printf(
					"ERROR: %s: wrong condition found. Arguments not in format: -<key> [<value>]\n",
					handler_name);

			return -1;
		}

		if (args_count >= MAX_SHELL_KEYS) {
			printf("ERROR: %s: wrong key entered. See help.\n", handler_name);
			return -2;
		}
		// Get key name.
		// Second symbol. Ex: -h -x (-hello == -h)
		keys[args_count].name = *(*(argsv + i) + 1);
		keys[args_count].value = 0x0;

		// Now working with value if one exists
		i++;
		if (i < argsc && **(argsv + i) != '-') {
			keys[args_count].value = *(argsv + i);
			i++;
		}
		args_count++;
	}

	//return args_count;

	for (i = 0; i < args_count; i++) {
		for (j = 0; j < amount_of_available_keys; j++) {
			if (keys[i].name == available_keys[j]) {
				break;
			}
		}
		if (j >= amount_of_available_keys) {
			printf("ERROR: %s: incorrect key entered! See help.\n",
					handler_name);
			return -3;
		}
	}
	return args_count;
}

// Compare keys with available
// returns TRUE if all keys presented are available, FALSE otherwise
int check_keys(SHELL_KEY *keys, int amount, char *available_keys,
		int amount_of_available_keys) {
	int i, j;

	for (i = 0; i < amount; i++) {
		for (j = 0; j < amount_of_available_keys; j++) {
			if (keys[i].name == available_keys[j]) {
				break;
			}
		}
		if (j >= amount_of_available_keys) {
			return FALSE;
		}
	}
	return TRUE;
}

// Determines whether key was entered
// returns TRUE if key is active (was entered by user) and fills value
int get_key(char key, SHELL_KEY *keys, int keys_amount, char **value) {
	int i;
	for (i = 0; i < keys_amount; i++) {
		if (keys[i].name == key) {
			*value = keys[i].value;
			return TRUE;
		}
	}
	return FALSE;
}
