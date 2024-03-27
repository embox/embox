/**
 * @file
 * @brief Serial driver for x86 (compatible with 16550)
 *
 * @date 12.04.10
 * @author Nikolay Korotky
 */

#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include <drivers/diag.h>

//#include <drivers/common/memory.h>

#include <framework/mod/options.h>


/** Default I/O addresses
 * NOTE: The actual I/O addresses used are stored
 *       in a table in the BIOS data area 0000:0400.
 */

#define UART_BASE      OPTION_GET(NUMBER, base_addr)
#define BAUD_RATE      OPTION_GET(NUMBER,baud_rate)

extern const struct uart_ops i8250_uart_ops;

static struct uart uart_diag = {
		.uart_ops = &i8250_uart_ops,
		.base_addr = UART_BASE,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart_diag, &uart_diag_params);

