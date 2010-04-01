/**
 * @file
 *
 * @date 02.02.2009
 * @author Alexey Fomin
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "console/console.h"
#include <shell.h>
#include <kernel/sys.h>
#include <shell_command.h>
#include <embox/unit.h>

// XXX just for now -- Eldar
EMBOX_UNIT(shell_start, shell_stop);

/* *str becomes pointer to first non-space character*/
void skip_spaces(char **str) {
	while (**str == ' ') {
		(*str)++;
	}
}

/* *str becomes pointer to first space or '\0' character*/
void skip_word(char **str) {
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

static void exec_callback(CONSOLE_CALLBACK *cb, CONSOLE *console, char *cmdline) {
	int words_counter = 0;

	SHELL_COMMAND_DESCRIPTOR *c_desc;
	char *words[CMDLINE_MAX_LENGTH + 1];

	if (0 == (words_counter = parse_str(cmdline, words))) {
		return; /* Only spaces were entered */
	}
	if(NULL == (c_desc = shell_command_descriptor_find_first(words[0], -1))){
		printf("%s: Command not found\n", words[0]);
		return;
	}
	if (NULL == c_desc->exec){
		LOG_ERROR("shell command: wrong command descriptor\n");
		return;
	}

	shell_command_exec(c_desc, words_counter, words);
	return;
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
	int start = cursor, i;
	char ch;
	SHELL_COMMAND_DESCRIPTOR * shell_desc;
	while (start > 0 && isalpha(line[start - 1])) {
		start--;
	}
	line += start;

	*offset = cursor - start;
	*proposals_len = 0;

	for(shell_desc = shell_command_descriptor_find_first((char*)line, *offset);
			NULL != shell_desc;
			shell_desc = shell_command_descriptor_find_next(shell_desc, (char *)line, *offset)){
		proposals[(*proposals_len)++] = (char *)shell_desc->name;
	}
	*common = 0;
	if (*proposals_len == 0) {
		return;
	}
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

static void shell_start_script(CONSOLE *console, CONSOLE_CALLBACK *callback ) {
	static char *script_commands[] = {
		#include <start_script.inc>;
	};

	char buf[CMDLINE_MAX_LENGTH + 1];
	int i;
	for (i = 0; i < array_len(script_commands); i++) {
		strncpy(buf, script_commands[i], sizeof(buf));
		printf("> %s \n", buf);
		exec_callback(callback, console, buf);
	}
}

static CONSOLE console[1];

static int shell_start(void) {
	static const char* prompt = CONFIG_SHELL_PROMPT;
	static CONSOLE_CALLBACK callback[1];

	callback->exec = exec_callback;
	callback->guess = guess_callback;
	if (console_init(console, callback) == NULL) {
		LOG_ERROR("Failed to create a console");
		return -1;
	}
	printf("\nStarting script...\n\n");
	shell_start_script(console, callback);

	printf("\n\n%s", CONFIG_SHELL_WELCOME_MSG);
	console_start(console, prompt);
	return 0;
}

static int shell_stop(void) {
	console_stop(console);
	return 0;
}
