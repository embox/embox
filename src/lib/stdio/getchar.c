/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/uart.h>
#include <kernel/diag.h>

int getchar_getc(void) {
	#ifndef CONFIG_HARD_DIAGUART
	return uart_getc();
	#else
	return diag_getc();
	#endif
}

int getchar_putc(int c) {
	#ifndef CONFIG_HARD_DIAGUART
	uart_putc((char) c);
	return 0;
	#else
	diag_putc((char) c);
	return 0;
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
	getchar_getc();
	return ch;
#else
# ifdef CONFIG_DRIVER_SUBSYSTEM
	extern int fungetc(FILE, int);
	return fungetc(stdin,ch);
# else
	/* default */
	getchar_getc();
	return ch;
# endif
#endif
}

