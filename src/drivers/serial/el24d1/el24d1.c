/**
 * @file el24d1.c
 * @brief UART driver for SAULT-EL24D1 board
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 
 * @date 14.11.2016
 */

#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(el24d1_uart_init);

#define UART_BASE OPTION_GET(NUMBER, base_addr)
#define UART(x)   (*(volatile uint32_t *)(UART_BASE + (x)))
#define IRQ_NUM OPTION_GET(NUMBER, irq_num)

#define RBR  0x00
#define THR  0x00
#define DLL  0x00
#define DLH  0x04
#define IER  0x04
#define IIR  0x08
#define FCR  0x08
#define LCR  0x0C
#define MCR  0x10
#define LSR  0x14
#define MSR  0x18
#define SCR  0x1C
#define SRBR 0x30
#define STHR 0x30
#define USR  0x7C
#define TFL  0x80
#define RFL  0x84
#define SRR  0x88
#define SRTS 0x8C
#define SBCR 0x90
#define SFE  0x98
#define SRT  0x9C
#define STET 0xA0
#define HTX  0XA4

#define USR_BUSY (1 << 0)
#define USR_TFNF (1 << 1)
#define LSR_RDR  (1 << 0)
#define LSR_THRE (1 << 5)

static int el24d1_uart_setup(struct uart *dev, const struct uart_params *params) {
	return 0;
}

static int el24d1_uart_has_symbol(struct uart *dev) {
	return UART(LSR) & LSR_RDR;
}

static int el24d1_uart_getc(struct uart *dev) {
	return (char)UART(RBR);
}

static int el24d1_uart_putc(struct uart *dev, int ch) {
	while (!(UART(LSR) & LSR_THRE)) {
	}

	UART(THR) = ch;

	return 0;
}

static const struct uart_ops el24d1_uart_uart_ops = {
		.uart_getc = el24d1_uart_getc,
		.uart_putc = el24d1_uart_putc,
		.uart_hasrx = el24d1_uart_has_symbol,
		.uart_setup = el24d1_uart_setup,
};

static struct uart uart0 = {
		.uart_ops = &el24d1_uart_uart_ops,
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

static int el24d1_uart_init(void) {
	return uart_register(&uart0, &uart_defparams);
}

PERIPH_MEMORY_DEFINE(el24d1_uart, UART_BASE, 0xB0);
