/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 */

#include <drivers/diag.h>
#include <drivers/iodev.h>

static int iodev_diag_init(void) {
	diag_init();
	return 0;
}

const struct iodev_ops iodev_diag_ops_struct = {
	.init = &iodev_diag_init,
	.getc = &diag_getc,
	.putc = &diag_putc,
	.kbhit = &diag_kbhit
};

const struct iodev_ops *const iodev_diag_ops = &iodev_diag_ops_struct;
