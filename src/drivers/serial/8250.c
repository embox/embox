/**
 * @file
 * @brief Serial driver for x86 (compatible with 16550)
 *
 * @date 12.04.10
 * @author Nikolay Korotky
 */

#include <stdint.h>
#include <asm/io.h>
#include <drivers/serial/8250.h>
#include <drivers/uart_device.h>
#include <embox/unit.h>


/** Default I/O addresses
 * NOTE: The actual I/O addresses used are stored
 *       in a table in the BIOS data area 0000:0400.
 */
#define COM0_PORT_BASE      0x3f8
#define COM0_IRQ_NUM        0x4

//#define COM1_PORT           0x2f8
//#define COM2_PORT           0x3e8
//#define COM3_PORT           0x2e8

#if 0
/* if we want disable all interrupts */
	out8(UART_DLAB, dev->base_addr + UART_LCR);
	/*disable all uart interrupts*/
	out8(0x0, dev->base_addr + UART_IER);
#endif

static uint8_t calc_line_stat(struct uart_params *params) {
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

static int uart_setup(struct uart_device *dev, struct uart_params *params) {
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
	out8(UART_IER_RX_ENABLE, dev->base_addr + UART_IER);

	return 0;
}

static int uart_putc(struct uart_device *dev, int ch) {
	while (!(in8(dev->base_addr + UART_LSR) & UART_EMPTY_TX));
	out8((uint8_t) ch, dev->base_addr + UART_TX);
	return 0;
}

static int uart_has_symbol(struct uart_device *dev) {
	return in8(dev->base_addr + UART_LSR) & UART_DATA_READY;
}

static int uart_getc(struct uart_device *dev) {
	return in8(dev->base_addr + UART_RX);
}

static struct uart_params uart0_params = {
		.baud_rate = OPTION_GET(NUMBER, baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8
};

static struct uart_ops uart_ops = {
		.get = uart_getc,
		.put = uart_putc,
		.hasrx = uart_has_symbol,
		.setup = uart_setup
};

static struct uart_device uart0 = {
		.dev_name = "uart0",
		.irq_num = COM0_IRQ_NUM,
		.base_addr = COM0_PORT_BASE,
		.params = &uart0_params,
		.operations = &uart_ops,
};

struct uart_device *diag_uart = &uart0;

#include <embox/device.h>

static int uart_init(void) {
	uart_dev_register(&uart0);
	return 0;
}

EMBOX_DEVICE("uart", &uart_dev_file_op, uart_init);
