/**
 * @file
 *
 * @date 21.04.2016
 * @author Anton Bondarev
 */

#ifndef DRIVERS_TTY_SERIAL_TTYS_H_
#define DRIVERS_TTY_SERIAL_TTYS_H_

#include <drivers/char_dev.h>
#include <drivers/tty.h>
#include <kernel/irq.h>
#include <kernel/task/resource/idesc.h>
#include <lib/libds/array.h>
#include <util/macro.h>

struct uart;

struct tty_uart {
	struct tty tty;
	struct uart *uart;
};

extern const struct idesc_ops idesc_serial_ops;

extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

#define TTYS_DEF(name, uart_dev)                                      \
	ARRAY_SPREAD_DECLARE(struct uart *const, __uart_device_registry); \
	ARRAY_SPREAD_ADD(__uart_device_registry, uart_dev);               \
	CHAR_DEV_DEF(name, &idesc_serial_ops, uart_dev)

#endif /* DRIVERS_TTY_SERIAL_TTYS_H_ */
