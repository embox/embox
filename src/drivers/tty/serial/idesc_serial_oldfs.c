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

#include "idesc_serial.h"

#include <framework/mod/options.h>

#define MAX_SERIALS \
	OPTION_GET(NUMBER, serial_quantity)

POOL_DEF(uart_ttys, struct tty_uart, MAX_SERIALS);

#define idesc_to_uart(desc) \
	(((struct  tty_uart*)desc)->uart)

extern struct tty_ops uart_tty_ops;
extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

static int idesc_uart_bind(struct uart *uart) {
	struct tty_uart *tu;

	tu = pool_alloc(&uart_ttys);
	if (!tu) {
		return -ENOMEM;
	}

	tty_init(&tu->tty, &uart_tty_ops);

	tu->uart = uart;
	uart->tty = &tu->tty;
	uart->tty->idesc = &tu->idesc;
	uart->irq_handler = uart_irq_handler;

	return 0;
}

static void idesc_uart_unbind(struct uart *uart) {
	struct tty_uart *tu;

	uart->tty->idesc = NULL;
	tu = member_cast_out(uart->tty, struct tty_uart, tty);
	pool_free(&uart_ttys, tu);
}

struct idesc *idesc_serial_create(struct uart *uart, int __oflags) {

	assert(uart);

	if (idesc_uart_bind(uart)) {
		return NULL;
	}

	idesc_init(uart->tty->idesc, idesc_serial_get_ops(), O_RDWR);

	return uart->tty->idesc;
}

void idesc_serial_close(struct idesc *idesc) {
	struct uart *uart;

	uart = idesc_to_uart(idesc);

	idesc_uart_unbind(uart);
}
