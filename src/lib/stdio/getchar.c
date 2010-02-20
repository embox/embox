/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/diag.h>

int getchar(void) {
	return (int) diag_getc();
}

int ungetchar(int ch) {
	diag_putc((char) ch);
	return ch;
}

