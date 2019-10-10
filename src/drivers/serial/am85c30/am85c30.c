/**
 * @file e2k.c
 * @brief Monocube serial driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.12.2017
 */

#include <stdint.h>

#include <asm/io.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>

#include "am85c30.h"

#include <embox/unit.h>

EMBOX_UNIT_INIT(uart_init);

#define UART_BASE   (uintptr_t)0x83200040
#define	IRQ_NUM     3 /* ZILOG_IRQ_DEFAULT 3 */ /* default IRQ # */

static inline void
am85c30_com_outb(uintptr_t iomem_addr, uint8_t data)
{
	e2k_write8(data, (UART_BASE + 1));
	wmb();
}

static inline uint8_t
am85c30_com_inb(uintptr_t iomem_addr)
{
	unsigned char data_val;
	data_val = e2k_read8((UART_BASE + 1));
	rmb();
	return (data_val);
}

static inline uint8_t
am85c30_com_inb_command(uintptr_t iomem_addr, uint8_t reg)
{
	uint8_t reg_val;
	if (reg != 0)
		e2k_write8(reg, (UART_BASE));
	wmb();
	reg_val = e2k_read8((UART_BASE));
	rmb();
	return (reg_val);
}

static inline void
am85c30_com_outb_command(uintptr_t iomem_addr, uint8_t reg, uint8_t val)
{
	if (reg != 0)
		e2k_write8(reg, (UART_BASE));
	e2k_write8(val, (UART_BASE));
	wmb();
}

static inline void am85c30_sync(void)
{
	(void)e2k_read8((UART_BASE));
	rmb();
}

static int am85c30_setup(struct uart *dev, const struct uart_params *params) {
	//unsigned long port;

	//port = UART_BASE;
	//uint8_t val;

	//val = am85c30_com_inb_command(port, AM85C30_RR0);

	//am85c30_com_outb_command(port, 0, 0);

	return 0;
}

int am85c30_putc(struct uart *dev, int ch) {
	unsigned long port;

	port = UART_BASE;
	while ((am85c30_com_inb_command(port, AM85C30_RR0) & AM85C30_D2) == 0)
		;
	am85c30_com_outb(port + 0x01, ch);
	am85c30_sync();

	return 0;
}

static int am85c30_has_symbol(struct uart *dev) {
	unsigned long port;

	port = UART_BASE;
	uint8_t val;

	val = am85c30_com_inb_command(port, AM85C30_RR0);

	return val & AM85C30_D0;
}

static int am85c30_getc(struct uart *dev) {
	unsigned long port;
	uint8_t ch;
	uint8_t r1;

	port = UART_BASE;
	while (am85c30_has_symbol(NULL) == 0) {
	}

	r1 = am85c30_com_inb_command((uintptr_t)port, AM85C30_RR1);
	ch = am85c30_com_inb((uintptr_t)(port+1));

	return ch;
}

static const struct uart_ops am85c30_uart_ops = {
	.uart_getc = am85c30_getc,
	.uart_putc = am85c30_putc,
	.uart_hasrx = am85c30_has_symbol,
	.uart_setup = am85c30_setup,
};

static struct uart uart0 = {
	.uart_ops = &am85c30_uart_ops,
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
