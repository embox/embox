/*
 * @file
 *
 * @date Mar 18, 2013
 * @author: Anton Bondarev
 */
#include <drivers/iodev.h>

int diag_console_init(void) {
	iodev_setup(iodev_diag_ops);
	iodev_init();
	return 0;
}
