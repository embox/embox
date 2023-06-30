/**
 * @file
 *
 * @date Feb 5, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_TTY_SERIAL_TTYS_STUB_H_
#define SRC_DRIVERS_TTY_SERIAL_TTYS_STUB_H_

#include <drivers/char_dev.h>

#define TTYS_DEF(name, uart_dev) \
	ARRAY_SPREAD_DECLARE(struct uart *const, __uart_device_registry); \
	ARRAY_SPREAD_ADD(__uart_device_registry, uart_dev); \
	CHAR_DEV_DEF(name, NULL, NULL, NULL, uart_dev)


#endif /* SRC_DRIVERS_TTY_SERIAL_TTYS_STUB_H_ */
