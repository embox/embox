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

#define __TTYS_NAME(_name) MACRO_CONCAT(__tty_device__, _name)

#define TTYS_DEF(_name, _uart_ptr)                                          \
	ARRAY_SPREAD_DECLARE(struct uart *const, __uart_device_registry);       \
	ARRAY_SPREAD_ADD(__uart_device_registry, _uart_ptr);                    \
	static struct serial_dev __TTYS_NAME(_name) = {                         \
	    .cdev = CHAR_DEV_INIT(__TTYS_NAME(_name).cdev, MACRO_STRING(_name), \
	        serial_dev_get_ops()),                                          \
	    .uart = _uart_ptr,                                                  \
	};                                                                      \
	CHAR_DEV_REGISTER((struct char_dev *)&__TTYS_NAME(_name))

struct uart;

struct serial_dev {
	struct char_dev cdev;
	struct tty tty;
	struct uart *uart;
};

extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);
extern int ttys_register(const char *name, struct uart *uart);

extern const struct tty_ops __uart_tty_ops;
#define uart_tty_get_ops() (&__uart_tty_ops)

extern const struct char_dev_ops __serial_cdev_ops;
#define serial_dev_get_ops() (&__serial_cdev_ops)

#endif /* DRIVERS_TTY_SERIAL_TTYS_H_ */
