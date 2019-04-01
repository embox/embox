/**
 * @file
 * @brief Implements serial driver for Raspberry PI
 *
 * See BCM2835-ARM-Peripherals.pdf, 13 chapter for details.
 *
 * @date 02.07.15
 * @author Vita Loginova
 */

#include <stdint.h>

#include <hal/reg.h>
#include <drivers/diag.h>

#include <framework/mod/options.h>

#define BAUD_RATE OPTION_GET(NUMBER, baud_rate) /* default 115200 */
#define UART_CLOCK 3000000
#define UART_DIVIDER UART_CLOCK/(16 * BAUD_RATE)
#define UART_FRACTIONAL 40 /* (.627 * 64) + 0.5 */

#define GPIO_OFFSET  0x20200000

/* Controls actuation of pull up/down to ALL GPIO pins. */
#define GPPUD        GPIO_OFFSET + 0x94

/* Controls actuation of pull up/down for specific GPIO pin. */
#define GPPUDCLK0    GPIO_OFFSET + 0x98

#define UART0_OFFSET GPIO_OFFSET + 0x00001000

#define UART0_DR     UART0_OFFSET + 0x00 /* Data Register  */
#define UART0_FR     UART0_OFFSET + 0x18 /* Flag register */
#define UART0_IBRD   UART0_OFFSET + 0x24 /* Integer Baud rate divisor */
#define UART0_FBRD   UART0_OFFSET + 0x28 /* Fractional Baud rate divisor */
#define UART0_LCRH   UART0_OFFSET + 0x2C /* Line Control register */
#define UART0_CR     UART0_OFFSET + 0x30 /* Control register */
#define UART0_IMSC   UART0_OFFSET + 0x38 /* Interrupt Mask Set Clear Register */
#define UART0_ICR    UART0_OFFSET + 0x44 /* Interrupt Clear Register */


static void delay(int32_t count) {
	while (count--)
		;
}


static int raspi_diag_setup(const struct diag *diag) {
	/* Disable UART0. */
	REG_STORE(UART0_CR, 0x00000000);

	/* Disable pull up/down for all GPIO pins. */
	REG_STORE(GPPUD, 0x00000000);
	delay(150);

	/* Disable pull up/down for pin 14,15. */
	REG_STORE(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);

	/* Write 0 to GPPUDCLK0 to make it take effect. */
	REG_STORE(GPPUDCLK0, 0x00000000);

	/* Clear pending interrupts. */
	REG_STORE(UART0_ICR, 0x7FF);

	REG_STORE(UART0_IBRD, UART_DIVIDER);
	REG_STORE(UART0_FBRD, UART_FRACTIONAL);

	/* Enable FIFO & 8 bit data transmissio (1 stop bit, no parity). */
	REG_STORE(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

	/* Mask all interrupts. */
	REG_STORE(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
						  (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

	/* Enable UART0, receive & transfer part of UART. */
	REG_STORE(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));

	return 0;
}

static void raspi_diag_putc(const struct diag *diag, char ch) {
	while (REG_LOAD(UART0_FR) & (1 << 5))
		;
	REG_STORE(UART0_DR, ch);
}

static char raspi_diag_getc(const struct diag *diag) {
	return REG_LOAD(UART0_DR);
}

static int raspi_diag_hasrx(const struct diag *diag) {
	return !(REG_LOAD(UART0_FR) & (1 << 4));
}

DIAG_OPS_DEF(
		.init = raspi_diag_setup,
		.putc = raspi_diag_putc,
		.getc = raspi_diag_getc,
		.kbhit = raspi_diag_hasrx,
);
