/**
 * @file
 *
 * @brief
 *
 * @date 28.11.2011
 * @author Anton Bondarev
 */

#include <drivers/diag.h>

static void diag_stub_putc(const struct diag *diag, char ch)  {
}

DIAG_OPS_DEF(
	.putc = diag_stub_putc,
);
