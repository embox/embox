/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */
#include <assert.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <util/err.h>

#include <mem/misc/pool.h>

#include <drivers/tty.h>
#include <drivers/ttys.h>
#include <drivers/serial/uart_device.h>

#include <fs/idesc.h>
#include <fs/dvfs.h>

#include "idesc_serial.h"

#include <framework/mod/options.h>

#define MAX_SERIALS \
	OPTION_GET(NUMBER, serial_quantity)

#define idesc_to_uart(desc) \
	(((struct  tty_uart*)desc)->uart)

POOL_DEF(uart_ttys, struct tty_uart, MAX_SERIALS);

extern struct tty_ops uart_tty_ops;
extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

struct idesc *idesc_serial_create(struct uart *uart, int __oflags) {
	struct tty_uart *tu;

	assert(uart);

	tu = pool_alloc(&uart_ttys);
	if (!tu) {
		return err_ptr(ENOMEM);
	}

	tty_init(&tu->tty, &uart_tty_ops);

	tu->uart = uart;
	uart->tty = &tu->tty;
	uart->tty->idesc = &tu->idesc;
	uart->irq_handler = uart_irq_handler;

	idesc_init(&tu->idesc, idesc_serial_get_ops(), __oflags);

	return &tu->idesc;
}

void idesc_serial_close(struct idesc *idesc) {
	pool_free(&uart_ttys, idesc);
}
