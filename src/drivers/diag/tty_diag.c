/**
 * @file
 * @brief
 *
 * @date 09.09.11
 * @author Anton Kozlov
 * @author Ilia Vaprol
 */

#include <drivers/diag.h>
#include <drivers/tty.h>
#include <drivers/iodev.h>

void diag_putc(char ch) {
	tty_putc(&diag_tty, ch);
}

void diag_init(void) {
	iodev_diag_ops->init();
}
