/**
 * @file
 *
 * @date May 19, 2023
 * @author Anton Bondarev
 */
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>

#define BASE_ADDR UINTMAX_C(OPTION_GET(NUMBER, base_addr))
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)

extern const struct uart_ops ns16550_uart_ops;

static struct uart ns16550_diag = {
    .uart_ops = &ns16550_uart_ops,
    .base_addr = BASE_ADDR,
};

static const struct uart_params ns16550_diag_params = {
    .baud_rate = BAUD_RATE,
    .uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&ns16550_diag, &ns16550_diag_params);

PERIPH_MEMORY_DEFINE(ns16550_diag, BASE_ADDR, 0x1000);
