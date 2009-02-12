/*
 * shell.c
 *
 *  Created on: 02.02.2009
 *      Author: Alexey Fomin
 */

#include "shell.h"
#include "string.h"
#include "tty.h"

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

static void skip_spaces(int *i, char *str) {
	while (str[*i] == ' ') {
		str[(*i)++] = 0;
	}
}

static void tty_callback(char *cmdline) {
	int handler_args_counter = 0;
	int i = 0, j;
	PSHELL_HANDLER phandler;

	if (cmdline != NULL) {
		char *handler_args[sz_length(cmdline) / 2];

		// cmdline becomes pointer to the first word
		skip_spaces(&i, cmdline);
		cmdline += i;
		i = 0;

		// reading command
		// cmdline points the first word, i - first space (after command name) index
		while (cmdline[i] != 0 && cmdline[i] != ' ') {
			i++;
		}

		// stub! - choosing correct handler
		skip_spaces(&i, cmdline);

		for (j = 0; j < sizeof(shell_handlers); j++) {
			if (sz_cmp(cmdline, shell_handlers[j].name)) {
				phandler = shell_handlers[j].phandler;
				break;
			}
		}
		// if handler not found:
		if (j == sizeof(shell_handlers)) {
			printf("%s: Command not found\n", cmdline);
			return;
		}

		// make cmdline to be a pointer to the first argument if last exists
		cmdline += i;
		i = 0;

		// reading arguments
		while (*cmdline != 0) {
			handler_args[handler_args_counter++] = cmdline;
			while (cmdline[i] != 0 && cmdline[i] != ' ') {
				i++;
			}
			skip_spaces(&i, cmdline);
			cmdline += i;
			i = 0;
		}

		phandler(handler_args_counter, handler_args);
	}
}
int stub(char *cmdline, char **proposals){
	return 0;
}
void shell_start() {
	tty_start(tty_callback, stub, welcome);
}
