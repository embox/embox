/**
 * @file

 * @date 01 june  2024
 * @author: Anton Bondarev
 */
#include <stdint.h>

#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define UARTCLK         OPTION_GET(NUMBER, uartclk)
#define USE_BOARD_CONF  OPTION_GET(BOOLEAN, use_bconf)

#if USE_BOARD_CONF
#include "uart_setup_hw_board_config.inc"
#else
static inline int uart_setup_hw(struct uart *dev) {
	return 0;
}
#endif /* USE_BOARD_CONF */

static void mikron_usart_set_baudrate(struct uart *dev) {


}

static int mikron_usart_irq_enable(struct uart *dev, 
									const struct uart_params *params) {
	// if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
	// 	REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	// }
	return 0;
}

static int mikron_usart_irq_disable(struct uart *dev,
									const struct uart_params *params) {
	// REG32_STORE(UART_IMSC(dev->base_addr), 0);

	return 0;
}

static int mikron_usart_setup(struct uart *dev, 
								const struct uart_params *params) {
	/* Disable uart. */
	// REG32_STORE(UART_CR(dev->base_addr), 0);

	uart_setup_hw(dev);

	// if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
	// 	REG32_STORE(UART_IMSC(dev->base_addr), IMSC_RXIM);
	// }

	mikron_usart_set_baudrate(dev);

	// /* Word len 8 bit. */
	// REG32_STORE(UART_LCRH(dev->base_addr), UART_WLEN_8BIT << UART_WLEN_SHIFT);

	// /* Enable uart. */
	// REG32_STORE(UART_CR(dev->base_addr), UART_UARTEN | UART_TXE | UART_RXE);

	return 0;
}

static int mikron_usart_putc(struct uart *dev, int ch) {
	// while (REG32_LOAD(UART_FR(dev->base_addr)) & FR_TXFF)
	// 	;

	// REG32_STORE(UART_DR(dev->base_addr), (uint32_t)ch);

	return 0;
}

static int mikron_usart_getc(struct uart *dev) {
	// return REG32_LOAD(UART_DR(dev->base_addr));
	return 0;
}

static int mikron_usart_has_symbol(struct uart *dev) {
//	return !(REG32_LOAD(UART_FR(dev->base_addr)) & FR_RXFE);
	return 0;
}

const struct uart_ops mikron_usart_ops = {
    .uart_getc = mikron_usart_getc,
    .uart_putc = mikron_usart_putc,
    .uart_hasrx = mikron_usart_has_symbol,
    .uart_setup = mikron_usart_setup,
    .uart_irq_en = mikron_usart_irq_enable,
    .uart_irq_dis = mikron_usart_irq_disable,
};
