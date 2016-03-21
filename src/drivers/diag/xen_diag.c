/**
 * @file
 * @brief
 *
 * @date 17.03.2012
 * @author Anton Kozlov
 */

#include <drivers/diag.h>

static int diag_xen_init(const struct diag *diag) {
	return 0;
}

static void diag_xen_putc(const struct diag *diag, char ch) {
}

DIAG_OPS_DECLARE(
	.init = diag_xen_init,
	.putc = diag_xen_putc,
);
