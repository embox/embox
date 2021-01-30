/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.08.2014
 */

#include <stdint.h>
#include <string.h>

#include <hal/reg.h>

#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>

#include "stm32_usart_conf_f0.h"


/*
 * RM0360, 23.4.4 "USART baud rate generation", 609.
 * Oversampling = 8.
 */
#define USART_BRR(USART_CLK,USART_BAUDRATE)             (2 * USART_CLK / USART_BAUDRATE)

static uint32_t get_usart_clk (usart_struct *USART) {
	return 8000000;
}

#define REG_RCC         0x40021000       // Doc: DS9773 Rev 4, 39/93.
#define REG_RCC_APB2ENR (REG_RCC + 0x18) // Doc: DocID025023 Rev 4, 125/779.
#define REG_RCC_APB1ENR (REG_RCC + 0x1C) // Doc: DocID025023 Rev 4, 125/779.

static void set_usart_pwr (usart_struct *USART) {
	switch ((uint32_t)USART) {
	case (uint32_t)USART1:
			REG32_ORIN(REG_RCC_APB2ENR, (1 << 14)); // Doc: DocID025023 Rev 4, 113/779.
			break;

	case (uint32_t)USART2:
			REG32_ORIN(REG_RCC_APB1ENR, (1 << 17)); // Doc: DocID025023 Rev 4, 115/779.
			break;

	case (uint32_t)USART3:
			REG32_ORIN(REG_RCC_APB1ENR, (1 << 18)); // Doc: DocID025023 Rev 4, 115/779.
			break;

	case (uint32_t)USART4:
			REG32_ORIN(REG_RCC_APB1ENR, (1 << 19)); // Doc: DocID025023 Rev 4, 115/779.
			break;

	case (uint32_t)USART5:
			REG32_ORIN(REG_RCC_APB1ENR, (1 << 20)); // Doc: DocID025023 Rev 4, 115/779.
			break;

	case (uint32_t)USART6:
			REG32_ORIN(REG_RCC_APB2ENR, (1 << 5)); // Doc: DocID025023 Rev 4, 114/779.
			break;

	default:
		break;
	}
}

static int stm32_uart_setup(struct uart *dev, const struct uart_params *params) {
	usart_struct *USART = (usart_struct *)dev->base_addr;
	uint32_t usart_clk = get_usart_clk(USART);

	set_usart_pwr(USART);

	USART->CR3 = (1 << 12);

	USART->BRR = (USART_BRR(usart_clk, dev->params.baud_rate) & 0xFFFFFFF0) |
			     ((USART_BRR(usart_clk, dev->params.baud_rate) & 0xF) >> 1);

	USART->CR1 = (1 << 15)|(1 << 3)|(1 << 2)|(1 << 0);

	if (dev->params.uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		USART->CR1 |= (1 << 5);
	}

	return 0;
}

static int stm32_uart_putc(struct uart *dev, int ch) {
	usart_struct *USART = (usart_struct *)dev->base_addr;

	while ((USART->ISR & (1 << 7)) != 0);
	USART->TDR =  (uint8_t)ch;

	return 0;
}

static int stm32_uart_hasrx(struct uart *dev) {
	usart_struct *USART = (usart_struct *)dev->base_addr;

	return USART->ISR & (1 << 5);
}

static int stm32_uart_getc(struct uart *dev) {
	usart_struct *USART = (usart_struct *)dev->base_addr;

	return USART->TDR & 0xFF;
}

static int stm32_uart_irq_en(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		usart_struct *USART = (usart_struct *)dev->base_addr;

		USART->CR1 |= (1 << 5);
	}
	return 0;
}

static int stm32_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		usart_struct *USART = (usart_struct *)dev->base_addr;

		USART->CR1 &= ~(1 << 5);
	}
	return 0;
}

const struct uart_ops stm32_uart_ops = {
		.uart_getc = stm32_uart_getc,
		.uart_putc = stm32_uart_putc,
		.uart_hasrx = stm32_uart_hasrx,
		.uart_setup = stm32_uart_setup,
		.uart_irq_en = stm32_uart_irq_en,
		.uart_irq_dis = stm32_uart_irq_dis,
};

static struct uart stm32_diag = {
		.uart_ops = &stm32_uart_ops,
		.irq_num = USARTx_IRQn,
		.base_addr = (unsigned long)USARTx,
};

static const struct uart_params diag_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&stm32_diag, &diag_defparams);
