/**
 * @file
 * @date 04 aug 2015
 * @author: Anton Bondarev
 */

#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>

#define UART_BASE OPTION_GET(NUMBER, base_addr)
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops mikron_usart_ops;

static struct uart uart0 = {
    .uart_ops = &mikron_usart_ops,
    .base_addr = UART_BASE,
};

static const struct uart_params uart_diag_params = {
    .baud_rate = BAUD_RATE,
    .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

PERIPH_MEMORY_DEFINE(mikron_usart, UART_BASE, 0x30);
