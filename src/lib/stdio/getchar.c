/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

int getchar_getc() {
	#ifndef CONFIG_HARD_DIAGUART
	return uart_getc();
	#else
	return diag_getc();
	#endif
}

int getchar_putc(int c) {
	#ifndef CONFIG_HARD_DIAGUART
	return uart_putc((char) c);
	#else
	return diag_putc((char) c);
	#endif
}

int getchar(void) {
#ifdef CONFIG_HARD_UART_OUT
	return (int) getchar_getc();
#else
# ifdef CONFIG_DRIVER_SUBSYSTEM
	return fgetc(stdin);
# else
	/* default */
	return (int) getchar_getc();
# endif
#endif
}

int ungetchar(int ch) {
#ifdef CONFIG_HARD_UART_OUT
	getchar_getc((char) ch);
	return ch;
#else
# ifdef CONFIG_DRIVER_SUBSYSTEM
	return fungetc(stdin,ch);
# else
	/* default */
	getchar_getc((char) ch);
	return ch;
# endif
#endif
}

