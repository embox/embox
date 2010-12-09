/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/diag.h>

int putchar(int c) {
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		diag_putc('\r');
	}
	diag_putc((char) c);

	return (prev = c);
}

