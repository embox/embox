/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

#ifdef CONFIG_HARD_UART_OUT
int long_long_name_for_only_this_file_putc(int c) {
	#ifndef CONFIG_HARD_DIAGUART
	return uart_putc((char) c);
	#else
	return diag_putc((char) c);
	#endif
}
#endif

int putchar(int c) {
#ifdef CONFIG_HARD_UART_OUT
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		long_long_name_for_only_this_file_putc('\r');
	}
	//diag_putc((char) c);
	long_long_name_for_only_this_file_putc((char) c);

	return (prev = c);
#else
	return fputc(stdout,c);
#endif
}

