/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>
#include <kernel/diag.h>
#include <drivers/vconsole.h>

#ifdef CONFIG_TTY_CONSOLE_COUNT
int getchar(void) {
	return vconsole_getchar( cur_console );
}

int ungetchar(int ch) {
	getchar();
	return ch;
}

#else
int getchar(void) {
	return diag_getc();
}

int ungetchar(int ch) {
	getchar();
	return ch;
}
#endif
