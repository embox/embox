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
		/* svv: Excellent. I'm looking forward for something like
		 * 	cat file1 file2 > file3
		 */
	if (c == '\n' && prev != '\r') {
		ch = '\r';
		write(1, &ch, 1);
	}
	ch = c;
	write(1, &ch, 1);
	return (prev = c);
}
