/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

int getchar(void) {
#ifdef CONFIG_HARD_UART_OUT
	return (int) uart_getc();
#else
	return fgetc(stdin);
#endif
}

int ungetchar(int ch) {
#ifdef CONFIG_HARD_UART_OUT
	uart_putc((char) ch);
	return ch;
#else
	return fungetc(stdin,ch);
#endif
}

