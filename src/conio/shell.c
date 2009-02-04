/*
 * shell.c
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#include "shell.h"

#include "tty.h"

static const char* welcome = "monitor> ";

static char* tty_callback(const char *entered) {
	if (entered[0]) {
		printf("Stub! You've entered: %s\n", entered);
	}
	return (char*) welcome;
}

void shell_start() {
	tty_start(tty_callback);
}
