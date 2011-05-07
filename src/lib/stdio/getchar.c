/**
 * @file
 * @brief TODO
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/diag.h>
#include <drivers/vconsole.h>
#include <drivers/tty_action.h>

#ifdef CONFIG_TTY_CONSOLE_COUNT
int getchar(void) {
	return console_getchar();
}

int ungetchar(int ch) {
	getchar();
	return ch;
}

#else
int getchar(void) {
	return diag_getc();
}

int ungetchar(int ch) {
	getchar();
	return ch;
}
#endif
