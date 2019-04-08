/**
 * @file
 * @brief Serial driver for x86 (compatible with 16550)
 *
 * @date 12.04.10
 * @author Nikolay Korotky
 */

#include <stdint.h>
#include <asm/io.h>
#include <drivers/diag.h>
#include <drivers/serial/8250.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(uart_init);

/** Default I/O addresses
 * NOTE: The actual I/O addresses used are stored
 *       in a table in the BIOS data area 0000:0400.
 */
#define COM0_PORT_BASE      OPTION_GET(NUMBER,base_addr)
#define COM0_IRQ_NUM        OPTION_GET(NUMBER,irq_num)

static uint8_t calc_line_stat(const struct uart_params *params) {
	uint8_t line_stat;

	line_stat = 0;
	if(0 == params->parity) {
		line_stat |= UART_NO_PARITY;
	}
	if(8 == params->n_bits) {
		line_stat |= UART_8BITS_WORD;
	}
	if(1 == params->n_stop) {
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
	if (params->irq) {
		/*enable rx interrupt*/
		out8(UART_IER_RX_ENABLE, dev->base_addr + UART_IER);
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

static const struct uart_ops i8250_uart_ops = {
		.uart_getc = i8250_getc,
		.uart_putc = i8250_putc,
		.uart_hasrx = i8250_has_symbol,
		.uart_setup = i8250_setup,
};

static struct uart uart0 = {
		.uart_ops = &i8250_uart_ops,
		.irq_num = COM0_IRQ_NUM,
		.base_addr = COM0_PORT_BASE,
};

static const struct uart_params uart_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = true,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = false,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

static int uart_init(void) {
	return uart_register(&uart0, &uart_defparams);
}
