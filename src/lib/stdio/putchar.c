/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

int putchar(int c) {
#ifdef CONFIG_HARD_UART_OUT
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		uart_putc('\r');
	}
	//diag_putc((char) c);
	uart_putc((char) c);

	return (prev = c);
#else
	return fputc(stdout,c);
#endif
}

