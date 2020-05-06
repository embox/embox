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
	SEGGER_RTT_Write(0, &ch, 1);
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
