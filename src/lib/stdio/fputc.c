/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/uart.h>

int fputc(FILE f, int c) {
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		uart_putc('\r');
	}
	//diag_putc((char) c);
	uart_putc((char) c);

	return (prev = c);
}

