/**
 * @file
 *
 * @date Jul 28, 2023
 * @author Anton Bondarev
 */

#include <kernel/irq.h>

#include <util/macro.h>

#include <drivers/serial/uart_dev.h>
#include <drivers/ttys.h>

//#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#define UART_BASE      OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM        OPTION_GET(NUMBER, irq_num)
#define BAUD_RATE      OPTION_GET(NUMBER, baud_rate)

#define TTY_NAME    ttyS2

extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

extern const struct uart_ops i8250_uart_ops;

static struct uart i8250_uart_ttyS2 = {
		.uart_ops = &i8250_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = UART_BASE,
		.params =  {
				.baud_rate = BAUD_RATE,
				.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_USE_IRQ,
		},
};

//PERIPH_MEMORY_DEFINE(i8250_uart, UART_BASE, 0x1000);

STATIC_IRQ_ATTACH(IRQ_NUM, uart_irq_handler, &i8250_uart_ttyS2);

TTYS_DEF(TTY_NAME, &i8250_uart_ttyS2);
