/*
 * shell.c
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#include "shell.h"

#include "tty.h"

static const char* welcome = "monitor> ";

static void tty_callback(char *cmdline) {
	if (cmdline != NULL) {
		printf("Stub! You've entered: %s\n", cmdline);
	}
}

void shell_start() {
	tty_start(tty_callback, welcome);
}
