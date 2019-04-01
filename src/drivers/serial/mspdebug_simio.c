/**
 * @file 
 * @brief mspdebug simio console driver
 *
 * @author  Anton Kozlov 
 * @date    06.05.2015
 */

#include <hal/reg.h>
#include <drivers/diag.h>

#define MSPDEBUG_CONSOLE_BASE 0x00ff

static int mspdebug_diag_init(const struct diag *diag) {
	return 0;
}

static void mspdebug_diag_putc(const struct diag *diag, char ch) {
	REG_STORE(MSPDEBUG_CONSOLE_BASE, ch);
}

static char mspdebug_diag_getc(const struct diag *diag) {
	return 0;
}

static int mspdebug_diag_kbhit(const struct diag *diag) {
	return 0;
}

DIAG_OPS_DEF(
		.init = mspdebug_diag_init,
		.putc = mspdebug_diag_putc,
		.getc = mspdebug_diag_getc,
		.kbhit = mspdebug_diag_kbhit,
);
