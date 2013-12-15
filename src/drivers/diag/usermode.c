/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    01.03.2013
 */

#include <kernel/host.h>
#include <drivers/diag.h>
#include <embox/unit.h>

static void um_diag_putc(const struct diag *diag, char ch) {
	host_putchar(ch);
}

static char um_diag_getc(const struct diag *diag) {
	return host_getchar();
}

DIAG_OPS_DECLARE(
		.putc = um_diag_putc,
		.getc = um_diag_getc,
);

