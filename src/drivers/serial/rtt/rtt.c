/**
 * @file
 * @brief SEGGER RTT
 *
 * @author  Alexander Kalmuk
 * @date    06.05.2020
 */

#include <drivers/diag.h>
#include "SEGGER_RTT.h"

static int rtt_diag_init(const struct diag *diag) {
	SEGGER_RTT_Init();
	return 0;
}

static void rtt_diag_putc(const struct diag *diag, char ch) {
	/* RTT lost charachters, so we should check whether there is a room
	 * for a char, but on the other hand, we can block forever here if
	 * we are running without JLINK attached (nobody reads buffer and overflow occurs).
	 * So just put char without checking for possible overflows. */
#if 0
	int ret;

	do {
		ret = SEGGER_RTT_PutChar(0, ch);
	} while (!ret);
#else
	SEGGER_RTT_PutChar(0, ch);
#endif
}

static char rtt_diag_getc(const struct diag *diag) {
	return 0;
}

static int rtt_diag_kbhit(const struct diag *diag) {
	return 0;
}

DIAG_OPS_DEF(
		.init = rtt_diag_init,
		.putc = rtt_diag_putc,
		.getc = rtt_diag_getc,
		.kbhit = rtt_diag_kbhit,
);
