/**
 * @file esh.c
 *
 * @brief simple embox shell
 *
 * @date 13.11.2010
 * @author Fedor Burdun
 */

#include <embox/unit.h>
#include <kernel/printk.h>
#include <shell_command.h>
#include <lib/readline.h>
#include <stdio.h>

#define CMDLINE_MAX_LENGTH 127

/* TO-DO:
 	1) must work without embox.cmds.shell
	2) some troubles with testreadline exit
*/

#if 1 /* while linked with old embox shell. Where must be located its functions ???? */
/* *str becomes pointer to first non-space character*/
static void skip_spaces(char **str) {
	while (**str == ' ') {
		(*str)++;
	}
}

/* *str becomes pointer to first space or '\0' character*/
static void skip_word(char **str) {
	while (**str != '\0' && **str != ' ') {
		(*str)++;
	}
}

static int parse_str(char *cmdline, char **words) {
	size_t cnt = 0;
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
#endif

static int esh_start(void) {

	int words_counter = 0;
	int ret_code;
	SHELL_COMMAND_DESCRIPTOR *c_desc;
	char *words[CMDLINE_MAX_LENGTH + 1];
	char *cmdline;

//	FILE *ff = fopen("/dev/uart","r");

	for (;;) {
		cmdline = readline(CONFIG_SHELL_PROMPT);

		if (0 == (words_counter = parse_str(cmdline, words))) {
			continue; /* Only spaces were entered */
		}

		if (NULL == (c_desc = shell_command_descriptor_find_first(words[0], -1))) {
			printf("%s: Command not found\n", words[0]);
			continue;
		}

		if (NULL == c_desc->exec) {
			LOG_ERROR("shell command: wrong command descriptor\n");
			continue;
		}
		shell_command_exec(c_desc, words_counter, words);
		freeline(cmdline);
	}

//	fclose(ff);

	return 0;
}

static int esh_stop(void) {
	return 0;
}

EMBOX_UNIT(esh_start, esh_stop);

/* tmp */

