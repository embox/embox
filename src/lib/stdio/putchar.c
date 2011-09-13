/**
 * @file
 * @brief TODO
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

/* FIXME may be refactoring this using interfaces */

#include <stdio.h>
#include <unistd.h>
#include <kernel/diag.h>
#include <drivers/vconsole.h>
#include <drivers/tty_action.h>

int putchar(int c) {
	static char prev = 0;
	char ch;
	if (c == '\n' && prev != '\r') {
		ch = '\r';
		write(1, &ch, 1);
	}
	ch = c;
	write(1, &ch, 1);
	return (prev = c);
}
