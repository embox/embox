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
#include <shell_utils.h>

#define CMDLINE_MAX_LENGTH 127

void esh_run(void) {
	int words_counter = 0;
	SHELL_COMMAND_DESCRIPTOR *c_desc;
	char *words[CMDLINE_MAX_LENGTH + 1];
	char *cmdline;

	printf("\n%s\n",CONFIG_SHELL_WELCOME_MSG);

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
		printf("\n"); /* any command don't print \n in end */
		freeline(cmdline);
	}
}

static int esh_start(void) {
	printk("ESH: ");
#ifndef CONFIG_TTY_CONSOLE_COUNT
	esh_run();
#else
	scheduler_start();
#endif
	printk(" [ done ]\n");
	return 0;
}

static int esh_stop(void) {
	return 0;
}

EMBOX_UNIT(esh_start, esh_stop);

