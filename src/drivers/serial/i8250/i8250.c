/**
 * @file
 * @brief Serial driver for x86 (compatible with 16550)
 *
 * @date 12.04.10
 * @author Nikolay Korotky
 */

#include <stdint.h>

#include <asm/io.h>

#include <drivers/serial/uart_dev.h>

#include "i8250.h"

static uint8_t calc_line_stat(const struct uart_params *params) {
	uint8_t line_stat;
	uint32_t flags;

	line_stat = 0;
	flags = params->uart_param_flags;

	if((flags & UART_PARAM_FLAGS_PARITY_NONE)) {
		line_stat |= UART_NO_PARITY;
	}
	if(flags & UART_PARAM_FLAGS_8BIT_WORD) {
		line_stat |= UART_8BITS_WORD;
	}
	if((flags & UART_PARAM_FLAGS_2_STOP)) {
		line_stat |= UART_2_STOP_BIT;
	} else {
		line_stat |= UART_1_STOP_BIT;
	}
	return line_stat;
}

static int i8250_setup(struct uart *dev, const struct uart_params *params) {
	uint8_t line_stat;

	line_stat = calc_line_stat(params);

	line_stat = UART_NO_PARITY | UART_8BITS_WORD | UART_1_STOP_BIT;

	/* Turn off the interrupt */
	out8(0x0, dev->base_addr + UART_IER);
	/* Set DLAB */
	out8(UART_DLAB, dev->base_addr + UART_LCR);
	/* Set the baud rate */
	out8(DIVISOR(params->baud_rate) & 0xFF, dev->base_addr + UART_DLL);
	out8((DIVISOR(params->baud_rate) >> 8) & 0xFF, dev->base_addr + UART_DLH);
	/* Set the line status */
	out8(line_stat, dev->base_addr + UART_LCR);
	/* Uart enable FIFO */
	out8(UART_ENABLE_FIFO, dev->base_addr + UART_FCR);
	/* Uart enable modem (turn on DTR, RTS, and OUT2) */
	out8(UART_ENABLE_MODEM, dev->base_addr + UART_MCR);

	/*enable rx interrupt*/
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		/*enable rx interrupt*/
		out8(UART_IER_RX_ENABLE, dev->base_addr + UART_IER);
	}

	return 0;
}

static int i8250_irq_en(struct uart *dev, const struct uart_params *params) {
	/*enable rx interrupt*/
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		/*enable rx interrupt*/
		out8(UART_IER_RX_ENABLE, dev->base_addr + UART_IER);
	}

	return 0;
}

static int i8250_irq_dis(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		/*disable rx interrupt*/
		out8(0, dev->base_addr + UART_IER);
	}

	return 0;
}

static int i8250_putc(struct uart *dev, int ch) {
	while (!(in8(dev->base_addr + UART_LSR) & UART_EMPTY_TX));
	out8((uint8_t) ch, dev->base_addr + UART_TX);
	return 0;
}

static int i8250_has_symbol(struct uart *dev) {
	return in8(dev->base_addr + UART_LSR) & UART_DATA_READY;
}

static int i8250_getc(struct uart *dev) {
	return in8(dev->base_addr + UART_RX);
}

const struct uart_ops i8250_uart_ops = {
		.uart_getc = i8250_getc,
		.uart_putc = i8250_putc,
		.uart_hasrx = i8250_has_symbol,
		.uart_setup = i8250_setup,
		.uart_irq_en = i8250_irq_en,
		.uart_irq_dis = i8250_irq_dis,
};

