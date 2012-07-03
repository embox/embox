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

static char ungetch;
static char has_ungetch = 0;

int getchar(void) {
	if (has_ungetch) {
		has_ungetch = 0;
		return ungetch;
	}

	return diag_getc();
}

int ungetchar(int ch) {
	ungetch = ch;
	has_ungetch = 1;
	return ch;
}
