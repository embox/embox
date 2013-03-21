/**
 * @file
 *
 * @date Mar 18, 2013
 * @author: Anton Bondarev
 */
#include <drivers/diag.h>
#include <drivers/iodev.h>
#include <drivers/uart_device.h>


struct uart_device diag_uart;
struct tty *diag_tty = &diag_uart.tty;
static int init_diag(void) {
	diag_init();
	return 0;
}

static const struct iodev_ops iodev_diag_ops_struct = {
	.init = &init_diag,
	.getc = &diag_getc,
	.putc = &diag_putc,
	.kbhit = &diag_kbhit,
};

int diag_console_init(void) {
	iodev_setup(&iodev_diag_ops_struct);
	iodev_init();
	return 0;
}
