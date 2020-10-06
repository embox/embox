/**
 * @file
 *
 * @date 09 may 2019
 * @author: Dmitry Kurbatov
 */

#include <stdint.h>

#include <drivers/diag.h>

#include <hal/reg.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include <drivers/ttys.h>

#include <framework/mod/options.h>

#define UART_BASE        OPTION_GET(NUMBER,base_addr)
#define IRQ_NUM          OPTION_GET(NUMBER,irq_num)

#define UART_CLOCK_FREQ  OPTION_GET(NUMBER,clock_freq)
#define UART_BAUD_RATE   OPTION_GET(NUMBER,baud_rate)

/*
 * Register offsets
 */

/* TXDATA */
#define SIFIVE_SERIAL_TXDATA_OFFS        0x0
#define SIFIVE_SERIAL_TXDATA_FULL_SHIFT  31
#define SIFIVE_SERIAL_TXDATA_FULL_MASK  (1 << SIFIVE_SERIAL_TXDATA_FULL_SHIFT)
#define SIFIVE_SERIAL_TXDATA_DATA_SHIFT  0
#define SIFIVE_SERIAL_TXDATA_DATA_MASK  (0xff << SIFIVE_SERIAL_TXDATA_DATA_SHIFT)

/* RXDATA */
#define SIFIVE_SERIAL_RXDATA_OFFS         0x4
#define SIFIVE_SERIAL_RXDATA_EMPTY_SHIFT  31
#define SIFIVE_SERIAL_RXDATA_EMPTY_MASK  (1 << SIFIVE_SERIAL_RXDATA_EMPTY_SHIFT)
#define SIFIVE_SERIAL_RXDATA_DATA_SHIFT   0
#define SIFIVE_SERIAL_RXDATA_DATA_MASK   (0xff << SIFIVE_SERIAL_RXDATA_DATA_SHIFT)

/* TXCTRL */
#define SIFIVE_SERIAL_TXCTRL_OFFS         0x8
#define SIFIVE_SERIAL_TXCTRL_TXCNT_SHIFT  16
#define SIFIVE_SERIAL_TXCTRL_TXCNT_MASK  (0x7 << SIFIVE_SERIAL_TXCTRL_TXCNT_SHIFT)
#define SIFIVE_SERIAL_TXCTRL_NSTOP_SHIFT  1
#define SIFIVE_SERIAL_TXCTRL_NSTOP_MASK  (1 << SIFIVE_SERIAL_TXCTRL_NSTOP_SHIFT)
#define SIFIVE_SERIAL_TXCTRL_TXEN_SHIFT   0
#define SIFIVE_SERIAL_TXCTRL_TXEN_MASK   (1 << SIFIVE_SERIAL_TXCTRL_TXEN_SHIFT)

/* RXCTRL */
#define SIFIVE_SERIAL_RXCTRL_OFFS         0xC
#define SIFIVE_SERIAL_RXCTRL_RXCNT_SHIFT  16
#define SIFIVE_SERIAL_RXCTRL_RXCNT_MASK  (0x7 << SIFIVE_SERIAL_TXCTRL_TXCNT_SHIFT)
#define SIFIVE_SERIAL_RXCTRL_RXEN_SHIFT   0
#define SIFIVE_SERIAL_RXCTRL_RXEN_MASK   (1 << SIFIVE_SERIAL_RXCTRL_RXEN_SHIFT)

/* IE */
#define SIFIVE_SERIAL_IE_OFFS             0x10
#define SIFIVE_SERIAL_IE_RXWM_SHIFT       1
#define SIFIVE_SERIAL_IE_RXWM_MASK       (1 << SIFIVE_SERIAL_IE_RXWM_SHIFT)
#define SIFIVE_SERIAL_IE_TXWM_SHIFT       0
#define SIFIVE_SERIAL_IE_TXWM_MASK       (1 << SIFIVE_SERIAL_IE_TXWM_SHIFT)

/* IP */
#define SIFIVE_SERIAL_IP_OFFS             0x14
#define SIFIVE_SERIAL_IP_RXWM_SHIFT       1
#define SIFIVE_SERIAL_IP_RXWM_MASK       (1 << SIFIVE_SERIAL_IP_RXWM_SHIFT)
#define SIFIVE_SERIAL_IP_TXWM_SHIFT       0
#define SIFIVE_SERIAL_IP_TXWM_MASK       (1 << SIFIVE_SERIAL_IP_TXWM_SHIFT)

/* DIV */
#define SIFIVE_SERIAL_DIV_OFFS            0x18
#define SIFIVE_SERIAL_DIV_DIV_SHIFT       0
#define SIFIVE_SERIAL_DIV_DIV_MASK       (0xffff << SIFIVE_SERIAL_IP_DIV_SHIFT)

static int sifive_uart_setup(struct uart *dev, const struct uart_params *params) {
	REG32_STORE(dev->base_addr + SIFIVE_SERIAL_DIV_OFFS,
			(UART_CLOCK_FREQ / UART_BAUD_RATE - 1));

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(dev->base_addr + SIFIVE_SERIAL_IE_OFFS, SIFIVE_SERIAL_IE_RXWM_MASK);
	} else {
		REG32_STORE(dev->base_addr + SIFIVE_SERIAL_IE_OFFS, 0);
	}

	REG32_STORE(dev->base_addr + SIFIVE_SERIAL_IP_OFFS, 0);

	REG32_STORE(dev->base_addr + SIFIVE_SERIAL_TXCTRL_OFFS,
			SIFIVE_SERIAL_TXCTRL_TXEN_MASK);
	REG32_STORE(dev->base_addr + SIFIVE_SERIAL_RXCTRL_OFFS,
			SIFIVE_SERIAL_RXCTRL_RXEN_MASK);
	return 0;
}

static int sifive_uart_hasrx(struct uart *dev) {
	uint32_t reg;

	reg = REG32_LOAD(dev->base_addr + SIFIVE_SERIAL_IP_OFFS);
	if (reg & SIFIVE_SERIAL_IP_RXWM_MASK) {
		return 1;
	}

	return 0;
}

static int sifive_uart_getc(struct uart *dev) {
	uint32_t reg;

	reg = REG32_LOAD(dev->base_addr + SIFIVE_SERIAL_RXDATA_OFFS);

	return reg & SIFIVE_SERIAL_RXDATA_DATA_MASK;
}

static int sifive_uart_putc(struct uart *dev, int ch) {
	while((REG32_LOAD(dev->base_addr + SIFIVE_SERIAL_TXDATA_OFFS) & SIFIVE_SERIAL_TXDATA_FULL_MASK));

	REG32_STORE(dev->base_addr + SIFIVE_SERIAL_TXDATA_OFFS,
			ch & SIFIVE_SERIAL_TXDATA_DATA_MASK);

	return 0;
}

static int sifive_uart_irq_en(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(dev->base_addr + SIFIVE_SERIAL_IE_OFFS, SIFIVE_SERIAL_IE_RXWM_MASK);
	}
	return 0;
}

static int sifive_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(dev->base_addr + SIFIVE_SERIAL_IE_OFFS, 0);
	}

	return 0;
}

static const struct uart_ops sifive_uart_ops = {
		.uart_getc = sifive_uart_getc,
		.uart_putc = sifive_uart_putc,
		.uart_hasrx = sifive_uart_hasrx,
		.uart_setup = sifive_uart_setup,
		.uart_irq_en = sifive_uart_irq_en,
		.uart_irq_dis = sifive_uart_irq_dis,
};

static struct uart sifive_diag = {
		.uart_ops = &sifive_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = (unsigned long) UART_BASE,
};

static const struct uart_params diag_defparams = {
		.baud_rate = UART_BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&sifive_diag, &diag_defparams);

static struct uart sifive_ttyS0 = {
		.uart_ops = &sifive_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = (unsigned long) UART_BASE,
		.params = {
				.baud_rate = UART_BAUD_RATE,
				.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_USE_IRQ,
		}
};

TTYS_DEF("ttyS0", &sifive_ttyS0);
