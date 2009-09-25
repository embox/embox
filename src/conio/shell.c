/**
 * \file shell.c
 *
 * \date 02.02.2009
 * \author Alexey Fomin
 */
#include "conio/conio.h"
#include "common.h"
#include "string.h"
#include "console.h"
#include "conio/shell.h"
#include "kernel/sys.h"
#include "shell_command.h"

static const char* welcome = MONITOR_PROMPT;

//static SHELL_HANDLER_DESCR shell_handlers[] = {
//#include "shell.inc"
//		};
//static SHELL_HANDLER_DESCR shell_handlers_old[] = {
//#include "shell.inc"
//		};

//static SHELL_HANDLER_DESCR shell_handlers[] = {
//#include "shell.inc"
//		};
//
//static int cur_shellhandlers_count = 0;
//
//void insert_shell_handler(char* name, char* descr, PSHELL_HANDLER func) {
//	shell_handlers[cur_shellhandlers_count].name = name;
//	shell_handlers[cur_shellhandlers_count].description = descr;
//	shell_handlers[cur_shellhandlers_count].phandler = func;
//	cur_shellhandlers_count++;
//}
//
//SHELL_HANDLER_DESCR *shell_get_command_list() {
//	return shell_handlers;
//}
//
//int shell_size_command_list() {
//	return cur_shellhandlers_count; //array_len(shell_handlers);
//}

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

static void exec_callback(CONSOLE_CALLBACK *cb, CONSOLE *console, char *cmdline) {
	int words_counter = 0;
	int i;
	PSHELL_HANDLER phandler;

	char *words[CMDLINE_MAX_LENGTH + 1];

	if (0 == (words_counter = parse_str(cmdline, words))) {
		// Only spaces were entered
		return;
	}

	// choosing correct handler
//	for (i = 0; i < cur_shellhandlers_count; i++) { //array_len(shell_handlers); i++) {
//		if (0 == strcmp(words[0], shell_handlers[i].name)) {
//			phandler = shell_handlers[i].phandler;
//			sys_exec_start(phandler, words_counter - 1, words + 1);
//			return;
//		}
//	}
	SHELL_COMMAND_DESCRIPTOR *c_desc;
	if(NULL == (c_desc = shell_command_descriptor_find_first(words[0], -1))){
		// handler not found:
		printf("%s: Command not found\n", words[0]);
		return;
	}
	if (NULL == c_desc->exec){
		printf ("Error shell command: wrong command descriptor\n");
		return;
	}
	//sys_exec_start(c_desc->exec, words_counter - 1, words + 1);
	shell_command_exec(c_desc, words_counter, words);
	return;
}

//TODO why it was placed here?
inline static BOOL is_char(char ch) {
	return ((ch > 0x20) && (ch < 0x7F));
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

//	for (i = 0; i < array_len(shell_handlers) && *proposals_len < max_proposals; i++) {
//		if (0 == strncmp(shell_handlers[i].name, line, *offset)) {
//			proposals[(*proposals_len)++] = shell_handlers[i].name;
//		}
//	}
	SHELL_COMMAND_DESCRIPTOR * shell_desc;
	for(shell_desc = shell_command_descriptor_find_first((char*)line, *offset);
			NULL != shell_desc;
			shell_desc = shell_command_descriptor_find_next(shell_desc, (char *)line, *offset)){
		proposals[(*proposals_len)++] = (char *)shell_desc->name;
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

static void shell_start_script(CONSOLE *console, CONSOLE_CALLBACK *callback ) {
	static char *script_commands[] = {
	#include "start_script.inc"
	};
	char buf[CMDLINE_MAX_LENGTH + 1];
	int i;
	for (i = 0; i < array_len(script_commands); i++) {
		strncpy(buf, script_commands[i], sizeof(buf));
		printf("> %s \n", buf);
		exec_callback(callback, console, buf);
	}
}

void shell_start() {
	static const char* prompt = MONITOR_PROMPT;
	static CONSOLE console[1];
	static CONSOLE_CALLBACK callback[1];

//	printf("cur_shellhandlers_count :%d \n",cur_shellhandlers_count);
//	int i;
//	for (i=0; i<array_len(shell_handlers_old); i++) {
//		shell_handlers[cur_shellhandlers_count] = shell_handlers_old[i];
//		cur_shellhandlers_count++;
//	}

	callback->exec = exec_callback;
	callback->guess = guess_callback;
	if (console_init(console, callback) == NULL) {
		LOG_ERROR("Failed to create a console");
		return;
	}
	printf("\nStarting script...\n\n");
	shell_start_script(console, callback);

	printf("\n\n%s", MONITOR_START_MSG);
	console_start(console, prompt);
}
