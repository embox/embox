/**
 * @file e2k.c
 * @brief Monocube serial driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.12.2017
 */

#include <asm/io.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(uart_init);

#define UART_BASE 0x83200040
#define IRQ_NUM 0

static int e2k_setup(struct uart *dev, const struct uart_params *params) {
	return 0;
}

static uint8_t _cmd(void *addr) {
	e2k_write8(0, addr);
	wmb();
	return e2k_read8(addr);
}

int e2k_putc(struct uart *dev, int ch) {
	int timeout = 0xfff;
	while (0 == (_cmd(UART_BASE) & (1 << 2)) && timeout--);
	e2k_write8(ch, (void *) UART_BASE + 1);

	return 0;
}

static int e2k_getc(struct uart *dev) {
	return 0;
}

static int e2k_has_symbol(struct uart *dev) {
	return 0;
}

static const struct uart_ops e2k_uart_ops = {
	.uart_getc = e2k_getc,
	.uart_putc = e2k_putc,
	.uart_hasrx = e2k_has_symbol,
	.uart_setup = e2k_setup,
};

static struct uart uart0 = {
	.uart_ops = &e2k_uart_ops,
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
