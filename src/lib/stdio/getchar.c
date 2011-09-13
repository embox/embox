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

int getchar(void) {
	return getc(stdin);
}

int ungetchar(int ch) {
	return ungetc(ch, stdin);
}
