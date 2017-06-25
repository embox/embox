/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    19.08.2013
 */

#ifndef DRIVERS_SERIAL_DIAG_H_
#define DRIVERS_SERIAL_DIAG_H_

#include <drivers/diag.h>

struct diag;
struct uart;
struct uart_params;

struct uart_diag {
	struct diag diag;

	struct uart *uart;
	const struct uart_params *params;
};

extern const struct diag_ops uart_diag_ops;

#endif /* DRIVERS_SERIAL_DIAG_H_ */

