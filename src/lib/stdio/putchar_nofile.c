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
#include <prom/diag.h>

int putchar(int c) {
	diag_putc(c);
	return c;
}
