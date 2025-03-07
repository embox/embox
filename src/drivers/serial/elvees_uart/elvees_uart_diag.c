/**
 * @file
 *
 * @date Feb 11, 2023
 * @author Anton Bondarev
 */

#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include <drivers/diag.h>

#include <drivers/common/memory.h>

#include <framework/mod/options.h>


#define UART_BASE      OPTION_GET(NUMBER, base_addr)
#define BAUD_RATE      OPTION_GET(NUMBER,baud_rate)

extern const struct uart_ops elvees_uart_uart_ops;

static struct uart uart_diag = {
		.uart_ops = &elvees_uart_uart_ops,
		.base_addr = UART_BASE,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart_diag, &uart_diag_params);

PERIPH_MEMORY_DEFINE(elvees_uart, UART_BASE, 0x1000);
