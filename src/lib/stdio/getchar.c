/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

#ifdef CONFIG_HARD_UART_OUT
int only_my_one_getc() {
	#ifndef CONFIG_HARD_DIAGUART
	return uart_getc();
	#else
	return diag_getc();
	#endif
}
#endif

#ifdef CONFIG_HARD_UART_OUT
int only_my_one_putc(int c) {
	#ifndef CONFIG_HARD_DIAGUART
	return uart_putc((char) c);
	#else
	return diag_putc((char) c);
	#endif
}
#endif

int getchar(void) {
#ifdef CONFIG_HARD_UART_OUT
	return (int) only_my_one_getc();
#else
	return fgetc(stdin);
#endif
}

int ungetchar(int ch) {
#ifdef CONFIG_HARD_UART_OUT
	only_my_one_putc((char) ch);
	return ch;
#else
	return fungetc(stdin,ch);
#endif
}

