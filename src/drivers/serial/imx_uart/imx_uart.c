/**
 * @file
 *
 * @data 01.04.2016
 * @author: Anton Bondarev
 */

#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(uart_init);

#define UART_BASE OPTION_GET(NUMBER, base_addr)
#define UART(x)   (*(volatile unsigned long *)(UART_BASE + (x)))
#define IRQ_NUM OPTION_GET(NUMBER, irq_num)

#define USR2_RDR   1
#define USR2_TXFE (1 << 14)

#define RXR  0x00
#define TXR  0x40
#define UCR1 0x80
#define UCR2 0x84
#define UCR3 0x88
#define UCR4 0x8C
#define UFCR 0x90
#define USR1 0x94 /* Status Register 1 */
#define USR2 0x98
#define UBIR 0xA4
#define UBMR 0xA8

#define UCR1_UARTEN (1 << 0)
#define UCR2_TXEN   (1 << 2)
#define UCR2_RXEN   (1 << 1)
#define UCR1_RRDYEN (1 << 9)
#define USR1_RRDY   (1 << 9)
#define USR1_RTSD       (1<<12) /* RTS delta */

static int imxuart_setup(struct uart *dev, const struct uart_params *params) {
	//UART(UCR1) = UCR1_UARTEN;
	//UART(UCR2) = 0x2127;//|= UCR2_RXEN | UCR2_TXEN;
	//UART(UCR3) = 0x0704;
	//UART(UCR4) = 0x7C00;
	if (params->irq) {
		uint32_t reg;

		reg = UART(UFCR);
		reg &= ~0x1F;
		reg |= 0x1; /* one character in RxFIFO */
		UART(UFCR) = reg;

		reg = UART(UCR1);
		reg |= UCR1_RRDYEN;
		UART(UCR1) = reg;
	}
	return 0;
}

static int imxuart_has_symbol(struct uart *dev) {
	return UART(USR2) & USR2_RDR;
}

static int imxuart_getc(struct uart *dev) {
	return (char)UART(RXR);
}

static int imxuart_putc(struct uart *dev, int ch) {
	while (!(UART(USR2) & USR2_TXFE)) {
	}

	UART(TXR) = ch;

	return 0;
}

static const struct uart_ops imxuart_uart_ops = {
		.uart_getc = imxuart_getc,
		.uart_putc = imxuart_putc,
		.uart_hasrx = imxuart_has_symbol,
		.uart_setup = imxuart_setup,
};

static struct uart uart0 = {
		.uart_ops = &imxuart_uart_ops,
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

const struct uart_diag DIAG_IMPL_NAME(__EMBUILD_MOD__) = {
		.diag = {
			.ops = &uart_diag_ops,
		},
		.uart = &uart0,
		.params = &uart_diag_params,
};

static int uart_init(void) {
	return uart_register(&uart0, &uart_defparams);
}

static struct periph_memory_desc imx_uart_mem = {
	.start = UART_BASE,
	.len   = 0xAC,
};

PERIPH_MEMORY_DEFINE(imx_uart_mem);
