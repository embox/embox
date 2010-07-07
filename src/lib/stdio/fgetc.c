/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/uart.h>

int fgetc(FILE f) {
	return (int) uart_getc();
}

int fungetc(FILE f, int ch) {
	uart_putc((char) ch);
	return ch;
}

