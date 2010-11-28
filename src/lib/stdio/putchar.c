/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/diag.h>

#ifdef CONFIG_HARD_UART_OUT
int putchar_putc(int c) {
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
		putchar_putc('\r');
	}
	putchar_putc((char) c);

	return (prev = c);
#else

# ifdef CONFIG_DRIVER_SUBSYSTEM
	/* uart_putc((char) c); */
	return fputc(stdout,c);
# else
/* #  error Libc.a: INCLUDE option DRIVER_SUBSYSTEM or HARD_UART_OUT for output in option-driver.conf. */
	/* default */
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		diag_putc('\r');
	}
	diag_putc((char) c);

	return (prev = c);
# endif
#endif
}

