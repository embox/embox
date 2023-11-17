/**
 * @file
 * @brief PrimeCell UART (PL011)
 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183g/DDI0183G_uart_pl011_r1p5_trm.pdf
 *
 * @date 04 aug 2015
 * @author: Anton Bondarev
 */
#include <stdint.h>

#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define UARTCLK         OPTION_GET(NUMBER, uartclk)

/* UART Registers */
#define UART_DR(base)   (base + 0x00)
#define UART_RSR(base)  (base + 0x04)
#define UART_ECR(base)  (base + 0x04)
#define UART_FR(base)   (base + 0x18)
#define UART_IBRD(base) (base + 0x24)
#define UART_FBRD(base) (base + 0x28)
#define UART_LCRH(base) (base + 0x2c)
#define UART_CR(base)   (base + 0x30)
#define UART_IMSC(base) (base + 0x38)
#define UART_MIS(base)  (base + 0x40)
#define UART_ICR(base)  (base + 0x44)

#define UART_FEN        (1 << 4)
#define UART_UARTEN     (1 << 0)
#define UART_TXE        (1 << 8)
#define UART_RXE        (1 << 9)
#define UART_WLEN_8BIT  0x3
#define UART_WLEN_SHIFT 5

/* Flag register */
#define FR_RXFE         0x10 /* Receive FIFO empty */
#define FR_TXFF         0x20 /* Transmit FIFO full */

#define IMSC_RXIM       (0x1 << 4)

static void pl011_set_baudrate(struct uart *dev) {
	/* FIXME Init baud rate only if UARTCLK is really used.
	 * Currenly it is not so for the teplates which use pl011. */
#if UARTCLK != 0
	int ibrd, fbrd;

	/* Baud Rate Divisor = UARTCLK/(16×Baud Rate) = BRDI + BRDF,
	 * See 2.4.3 UART operation.  */
	ibrd = (UARTCLK / (16 * BAUD_RATE));
	fbrd = ((UARTCLK % (16 * BAUD_RATE)) * 64) / (16 * BAUD_RATE);
	REG32_STORE(UART_IBRD, ibrd);
	REG32_STORE(UART_FBRD, fbrd);
#endif
}

static int pl011_irq_enable(struct uart *dev,
    const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	}
	return 0;
}

static int pl011_irq_disable(struct uart *dev,
    const struct uart_params *params) {
	REG32_STORE(UART_IMSC(dev->base_addr), 0);

	return 0;
}

static int pl011_setup(struct uart *dev, const struct uart_params *params) {
	/* Disable uart. */
	REG32_STORE(UART_CR(dev->base_addr), 0);

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	}

	pl011_set_baudrate(dev);

	/* Word len 8 bit. */
	REG32_STORE(UART_LCRH(dev->base_addr), UART_WLEN_8BIT << UART_WLEN_SHIFT);

	/* Enable uart. */
	REG32_STORE(UART_CR(dev->base_addr), UART_UARTEN | UART_TXE | UART_RXE);

	return 0;
}

static int pl011_putc(struct uart *dev, int ch) {
	while (REG32_LOAD(UART_FR(dev->base_addr)) & FR_TXFF)
		;

	REG32_STORE(UART_DR(dev->base_addr), (uint32_t)ch);

	return 0;
}

static int pl011_getc(struct uart *dev) {
	return REG32_LOAD(UART_DR(dev->base_addr));
}

static int pl011_has_symbol(struct uart *dev) {
	return !(REG32_LOAD(UART_FR(dev->base_addr)) & FR_RXFE);
}

const struct uart_ops pl011_uart_ops = {
    .uart_getc = pl011_getc,
    .uart_putc = pl011_putc,
    .uart_hasrx = pl011_has_symbol,
    .uart_setup = pl011_setup,
    .uart_irq_en = pl011_irq_enable,
    .uart_irq_dis = pl011_irq_disable,
};
