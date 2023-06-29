/**
 * @file
 * @data 04 aug 2015
 * @author: Anton Bondarev
 */

#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include <framework/mod/options.h>

#define UART_BASE OPTION_GET(NUMBER,base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER,irq_num)
#define UARTCLK   OPTION_GET(NUMBER,uartclk)
#define BAUD_RATE OPTION_GET(NUMBER,baud_rate)

extern const struct uart_ops pl011_uart_ops;

static struct uart uart0 = {
		.uart_ops = &pl011_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = UART_BASE,
};


static const struct uart_params uart_diag_params = {
		.baud_rate = BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

PERIPH_MEMORY_DEFINE(pl011, UART_BASE, 0x48);
