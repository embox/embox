/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.08.2014
 */

#include <stddef.h>

#include <kernel/irq.h>

#include <drivers/serial/stm_usart.h>

#include <drivers/serial/uart_device.h>
#include <drivers/ttys.h>

extern const struct uart_ops stm32_uart_ops;
extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

#define TTY_NAME    "ttyS1"

static struct uart stm32_ttySx = {
		.uart_ops = &stm32_uart_ops,
		.irq_num = USARTx_IRQn,
		.base_addr = (unsigned long) USARTx,
		.params = {
				.baud_rate = OPTION_GET(NUMBER,baud_rate),
				.parity = 0,
				.n_stop = 1,
				.n_bits = 8,
				.irq = true,
		}
};

TTYS_DEF(TTY_NAME, &stm32_ttySx);

STATIC_IRQ_ATTACH(USARTx_IRQn, uart_irq_handler, &stm32_ttySx);
