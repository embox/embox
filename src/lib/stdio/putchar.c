/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

/* FIXME may be refactoring this using interfaces */

#include <stdio.h>
#include <kernel/diag.h>
#include <drivers/vconsole.h>
#include <drivers/tty_action.h>

#ifdef CONFIG_TTY_CONSOLE_COUNT
int putchar(int c) {
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		console_putchar( '\r'); /* FIXME must be 'cur_tty->console[ cur_tty->console_cur ]' */
	}
	console_putchar( (char) c);

	return (prev = c);
}
#else
int putchar(int c) {
	static char prev = 0;

	if (c == '\n' && prev != '\r') {
		diag_putc('\r');
	}
	diag_putc((char) c);

	return (prev = c);
}
#endif

