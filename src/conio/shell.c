/*
 * shell.c
 *
 *  Created on: 02.02.2009
 *      Author: Alexey Fomin
 */
#include "types.h"
#include "common.h"
#include "string.h"
#include "console.h"
#include "shell.h"




static const char* welcome = "monitor> ";


static SHELL_HANDLER_DESCR shell_handlers[] = {
#include "shell.inc"
		};

SHELL_HANDLER_DESCR *shell_get_command_list(){
	return shell_handlers;
}
int shell_size_command_list()
{
	return sizeof(shell_handlers)/ sizeof(shell_handlers[0]);
}

/*
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
*/
//definition in header
//typedef struct {
//	char *name;
//	PSHELL_HANDLER phandler;
//} SHELL_HANDLER_DESCR;

//static SHELL_HANDLER_DESCR shell_handlers[] = {
//#include "shell.inc"
//		};


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
//TODO move from this place to arch folders
int shell_handler_process_create(PSHELL_HANDLER phandler, int argsc,
		char **argsv) {
	test_allow_aborting();
	shell_save_proc();
	__asm__(".global shell_handler_start\n"
			"shell_handler_start:\n\t"
	);
	phandler(argsc, argsv);

	__asm__(".global shell_handler_continue\n"
			"shell_handler_continue:\n\t"
	);
	shell_restore_proc();
	__asm__(".global shell_handler_exit\n"
			"shell_handler_exit:\n\t"
	);
	test_disable_aborting();
}

static void exec_callback(CONSOLE_CALLBACK *cb, CONSOLE *console, char *cmdline) {
	int words_counter = 0;
	int i;
	PSHELL_HANDLER phandler;

	char *words[1 + strlen(cmdline) / 2];

	if (0 == (words_counter = parse_str(cmdline, words))) {
		// Only spaces were entered
		return;
	}

	// choosing correct handler
	for (i = 0; i < array_len(shell_handlers); i++) {
		if (0 == strcmp(words[0], shell_handlers[i].name)) {
			phandler = shell_handlers[i].phandler;
			shell_handler_process_create(phandler, words_counter - 1, words + 1);
			return;
		}
	}
	// handler not found:
	printf("%s: Command not found\n", words[0]);

}

inline static BOOL is_char(char ch) {
	return (ch > 0x20) && (ch < 0x7F);
}

/**
 * Guesses command using its beginning
 *
 * -- Eldar
 */
static void guess_callback(CONSOLE_CALLBACK *cb, CONSOLE *console,
		const char* line, const int max_proposals, int *proposals_len,
		char *proposals[], int *offset, int *common) {
	int cursor = strlen(line);
	int start = cursor;
	while (start > 0 && is_char(line[start - 1])) {
		start--;
	}
	line += start;

	*offset = cursor - start;
	*proposals_len = 0;
	int i;

	for (i = 0; i < array_len(shell_handlers) && *proposals_len < max_proposals; i++) {
		if (0 == strncmp(shell_handlers[i].name, line, *offset)) {
			proposals[(*proposals_len)++] = shell_handlers[i].name;
		}
	}
	*common = 0;
	if (*proposals_len == 0) {
		return;
	}
	char ch;
	while (1) {
		if ((ch = proposals[0][*offset + *common]) == '\0') {
			return;
		}
		for (i = 1; i < *proposals_len; ++i) {
			if (ch != proposals[i][*offset + *common]) {
				return;
			}
		}
		(*common)++;
	}
}

void shell_start() {
	static const char* prompt = "monitor> ";
	static CONSOLE console[1];
	static CONSOLE_CALLBACK callback[1];
	callback->exec = exec_callback;
	callback->guess = guess_callback;
	if (console_init(console, callback) == NULL) {
		printf("Failed to create a console");
		return;
	}
	printf("\nWelcome to Monitor shell and have a lot of fun...");
	console_start(console, prompt);
}

/**
 * parse arguments array on keys-value structures and entered amount of entered keys otherwise (if everything's OK)
 * @return  -1 wrong condition found. Arguments not in format: -<key> [<value>]
 * @return  -2 too many keys entered
 * @return -3 wrong key
 * @return 0 if (if everything's OK)
 */
int parse_arg(const char *handler_name, int argsc, char **argsv,
		char *available_keys, int amount_of_available_keys, SHELL_KEY *keys) {
	int i, j, args_count;

	i = 0;
	args_count = 0;

	while (i < argsc) {
		if (*argsv[i] != '-') {
			ERROR("%s: wrong condition found. Arguments not in format: -<key> [<value>]\n",
					handler_name);

			return -1;
		}

		if (args_count >= MAX_SHELL_KEYS) {
			ERROR("%s: wrong key entered. See help.\n", handler_name);
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
			ERROR("%s: incorrect key entered! See help.\n",
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
