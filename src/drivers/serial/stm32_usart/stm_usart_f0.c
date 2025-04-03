/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    07.02.2021
 */

#include <stdint.h>
#include <string.h>

#include <hal/reg.h>

#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/gpio.h>

#include "stm32_usart_conf_f0.h"

extern const uint32_t HSE_VALUE;
#define HSI_VALUE 8000000

/*
 * RM0360, 23.4.4 "USART baud rate generation", 609.
 * Oversampling = 8.
 */
#define USART_BRR(USART_CLK,USART_BAUDRATE)             (2 * USART_CLK / USART_BAUDRATE)


// Clock part.
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t AHBRSTR;
    volatile uint32_t CFGR2;
    volatile uint32_t CFGR3;
    volatile uint32_t CR2;
} rcc_struct;

#define RCC       ((rcc_struct *)          0x40021000)

static uint32_t get_pll_clk () {
	uint32_t src_clk;

	if (RCC->CFGR & (1 << 16)) {
		src_clk = HSE_VALUE / (RCC->CFGR2 + 1);
	} else {
		src_clk = HSI_VALUE / 2;
	}

	src_clk *= ((RCC->CFGR >> 18) & 0b1111) + 2;
	return src_clk;
}

static uint32_t HPRE_VALUE[8] = {2, 4, 8, 16, 64, 128, 256, 512};
static uint32_t PPRE_VALUE[8] = {2, 4, 8, 16};

static uint32_t get_usart_clk (usart_struct *USART) {
	volatile uint32_t SW_SRC;

	switch ((RCC->CFGR & (0b11 << 2)) >> 2) {
	case 0b00:
		SW_SRC = HSI_VALUE;
		break;

	case 0b01:
		SW_SRC = HSE_VALUE;
		break;

	default:
		SW_SRC = get_pll_clk();
		break;
	}

	if (RCC->CFGR & (1 << 7)) {
		SW_SRC /= HPRE_VALUE[((RCC->CFGR >> 4) & 0b111)];
	}

	if (RCC->CFGR & (1 << 10)) {
		SW_SRC /= PPRE_VALUE[((RCC->CFGR >> 8) & 0b111)];
	}

	return SW_SRC;
}

#define REG_RCC         0x40021000       // Doc: DS9773 Rev 4, 39/93.
#define REG_RCC_APB2ENR (REG_RCC + 0x18) // Doc: DocID025023 Rev 4, 125/779.
#define REG_RCC_APB1ENR (REG_RCC + 0x1C) // Doc: DocID025023 Rev 4, 125/779.
#define REG_RCC_AHBENR  (REG_RCC + 0x14) // Doc: DocID025023 Rev 4, 125/779.

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

// GPIO part.
static void set_gpio_pwr (uint32_t port) {
	REG32_ORIN(REG_RCC_AHBENR, (1 << (port + 17))); // Doc: DocID025023 Rev 4, 111/779.
}

static gpio_struct *get_gpio (unsigned char port) {
	uint8_t *p = (uint8_t *)GPIOA;
	p += 1024 * port;
	return (gpio_struct *)p;
}

static void set_gpio_af (uint32_t port, uint32_t pin, uint32_t func) {
	gpio_struct* GPIO = get_gpio(port);

	if (pin < PINS_NUMBER / 2) {
		GPIO->AFRL &= ~L_AF_MSK(pin);
		GPIO->AFRL |= L_AF(pin, func);
	} else {
		GPIO->AFRH &= ~H_AF_MSK(pin);
		GPIO->AFRH |= H_AF(pin, func);
	}

	GPIO->MODER &= ~M_MSK(pin);
	GPIO->MODER |=  M_ALT(pin);
}

static void set_gpio_high_spped (uint32_t port, uint32_t pin) {
	gpio_struct* GPIO = get_gpio(port);

	GPIO->OSPEEDR &= ~S_MSK(pin);
	GPIO->OSPEEDR |= S_HS(pin);
}

static void init_usart_gpio () {
	set_gpio_pwr(OPTION_GET(NUMBER,port_tx));
	set_gpio_pwr(OPTION_GET(NUMBER,port_rx));

	set_gpio_af(OPTION_GET(NUMBER,port_tx), OPTION_GET(NUMBER,pin_tx), OPTION_GET(NUMBER,pin_tx_af));
	set_gpio_af(OPTION_GET(NUMBER,port_rx), OPTION_GET(NUMBER,pin_rx), OPTION_GET(NUMBER,pin_rx_af));

	set_gpio_high_spped(OPTION_GET(NUMBER,port_tx), OPTION_GET(NUMBER,pin_tx));
	set_gpio_high_spped(OPTION_GET(NUMBER,port_rx), OPTION_GET(NUMBER,pin_rx));
}

// USART part.
static int stm32_uart_setup(struct uart *dev, const struct uart_params *params) {
	init_usart_gpio();

	usart_struct *USART = (usart_struct *)dev->base_addr;
	volatile uint32_t usart_clk = get_usart_clk(USART);

	set_usart_pwr(USART);

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

	while ((USART->ISR & (1 << 7)) == 0);
	USART->TDR = (char)ch;

	return 0;
}

static int stm32_uart_hasrx(struct uart *dev) {
	usart_struct *USART = (usart_struct *)dev->base_addr;
	return USART->ISR & (1 << 5);
}

static int stm32_uart_getc(struct uart *dev) {
	usart_struct *USART = (usart_struct *)dev->base_addr;
	return USART->RDR & 0xFF;
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

// System part.
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
