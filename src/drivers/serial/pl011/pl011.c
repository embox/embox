/**
 * @file
 *
 * @data 04 aug 2015
 * @author: Anton Bondarev
 */
#include <stdint.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(uart_init);

#define UART_BASE OPTION_GET(NUMBER,base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER,irq_num)

/* UART Registers */
#define UART_DR		(UART_BASE + 0x00)
#define UART_RSR	(UART_BASE + 0x04)
#define UART_ECR	(UART_BASE + 0x04)
#define UART_FR		(UART_BASE + 0x18)
#define UART_IBRD	(UART_BASE + 0x24)
#define UART_FBRD	(UART_BASE + 0x28)
#define UART_LCRH	(UART_BASE + 0x2c)
#define UART_CR		(UART_BASE + 0x30)
#define UART_IMSC	(UART_BASE + 0x38)
#define UART_MIS	(UART_BASE + 0x40)
#define UART_ICR	(UART_BASE + 0x44)

/* Flag register */
#define FR_RXFE		0x10	/* Receive FIFO empty */
#define FR_TXFF		0x20	/* Transmit FIFO full */

#define IMSC_RXIM   (0x1 << 4)

static int pl011_setup(struct uart *dev, const struct uart_params *params) {
	if (params->irq) {
		REG_ORIN(UART_IMSC, IMSC_RXIM);
	}

	return 0;
}

static int pl011_putc(struct uart *dev, int ch) {
	while (REG_LOAD(UART_FR) & FR_TXFF) ;

	REG_STORE(UART_DR, (uint32_t)ch);

	return 0;
}

static int pl011_getc(struct uart *dev) {
	return REG_LOAD(UART_DR);
}

static int pl011_has_symbol(struct uart *dev) {
	return !(REG_LOAD(UART_FR) & FR_RXFE);
}

static const struct uart_ops pl011_uart_ops = {
		.uart_getc = pl011_getc,
		.uart_putc = pl011_putc,
		.uart_hasrx = pl011_has_symbol,
		.uart_setup = pl011_setup,
};

static struct uart uart0 = {
		.uart_ops = &pl011_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = UART_BASE,
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

PERIPH_MEMORY_DEFINE(pl011, UART_BASE, 0x48);
