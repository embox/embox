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
	host_write(1, &ch, 1);
}

static char um_diag_getc(const struct diag *diag) {
	int ret;
	char ch;

	do {
		ret = host_read(0, &ch, 1);
	} while (-1 == ret);

	return ch;
}

DIAG_OPS_DEF(
		.putc = um_diag_putc,
		.getc = um_diag_getc,
);

