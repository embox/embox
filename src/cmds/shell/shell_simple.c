/**
 */
#include <stdio.h>
#include <shell.h>
#include <embox/unit.h>
#include <shell_command.h>

EMBOX_UNIT(shell_start, shell_stop);

static int shell_start(void) {
	printf("\n\n%s", CONFIG_SHELL_WELCOME_MSG);

	printf("\n\n\e[1;34m Just for fun MESSAGE :D\e[0;0m\n\n");

	if (0)
	while (true) {
		putchar(getchar());
	}
	return 0;
}

static int shell_stop(void) {

	return 0;
}

