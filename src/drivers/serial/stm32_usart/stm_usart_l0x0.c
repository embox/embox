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
#include <kernel/irq.h>

#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>

#include "stm32_usart_conf_l0x0.h"

// Doc: DocID031151 Rev 1, RM0451, 620/774.
#define USART_BRR(USART_CLK,USART_BAUDRATE)             (2 * USART_CLK / USART_BAUDRATE)

// Clock part.
static const uint32_t HSI_VALUE = 16000000;
extern const uint32_t HSE_VALUE;

typedef struct {
    volatile uint32_t CR;        // 0x00
    volatile uint32_t ICSCR;     // 0x04
    volatile uint32_t RES_1;     // 0x08
    volatile uint32_t CFGR;      // 0x0C
    volatile uint32_t CIER;      // 0x10
    volatile uint32_t CIFR;      // 0x14
    volatile uint32_t CICR;      // 0x18
    volatile uint32_t IOPRSTR;   // 0x1C
    volatile uint32_t AHBRSTR;   // 0x20
    volatile uint32_t APB2RSTR;  // 0x24
    volatile uint32_t APB1RSTR;  // 0x28
    volatile uint32_t IOPENR;    // 0x2C
    volatile uint32_t AHBENR;    // 0x30
    volatile uint32_t APB2ENR;   // 0x34
    volatile uint32_t APB1ENR;   // 0x38
    volatile uint32_t IOPSMEN;   // 0x3C
    volatile uint32_t AHBSMENR;  // 0x40
    volatile uint32_t APB2SMENR; // 0x44
    volatile uint32_t APB1SMENR; // 0x48
    volatile uint32_t CCIPR;     // 0x4C
    volatile uint32_t CSR;       // 0x50
} rcc_struct; // Doc: DocID031151 Rev 1, RM0451, 185/774.

// Doc: DocID031151 Rev 1, RM0451, 39/774.
#define RCC       ((rcc_struct *)          0X40021000)

static uint32_t get_hsi_clk () {
	if (RCC->CR & (1 << 4)) {
		return HSI_VALUE / 4;
	} else {
		return HSI_VALUE;
	}
}

static uint32_t PLLMUL_VALUE[9] = {3, 4, 6, 8, 12, 16, 24, 32, 48};

static uint32_t get_pll_clk () {
	uint32_t src_clk;

	if (RCC->CFGR & (1 << 16)) {
		src_clk = HSE_VALUE;
	} else {
		src_clk = get_hsi_clk();
	}

	src_clk *= PLLMUL_VALUE[(RCC->CFGR >> 18) & 0b1111];
	src_clk /= ((RCC->CFGR >> 22) & 0b11) + 1;
	return src_clk;
}

static uint32_t HPRE_VALUE[8] = {2, 4, 8, 16, 64, 128, 256, 512};
static uint32_t PPRE_VALUE[8] = {2, 4, 8, 16};

static uint32_t MSI_CLI_TABLE[8] = {
		65536, 131072, 262144, 524288, 1048000, 2097000, 4194000, 0
};

static uint32_t get_msi_clk () {
	return MSI_CLI_TABLE[(RCC->ICSCR & (0b111 << 13)) >> 13];
}

static uint32_t get_usart_clk () {
	volatile uint32_t SW_SRC;

	switch ((RCC->CFGR & (0b11 << 2)) >> 2) {
	case 0b00:
		SW_SRC = get_msi_clk();
		break;

	case 0b01:
		SW_SRC = get_hsi_clk();
		break;

	case 0b10:
		SW_SRC = HSE_VALUE;
		break;

	case 0b11:
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

static void set_usart_pwr () {
	RCC->APB1ENR |= 1 << 17; // On USART2.
}

// GPIO part.
static void set_gpio_pwr (uint32_t port) {
	RCC->IOPENR |= (1 << port); // Doc: DocID031151 Rev 1, RM0451, 169/774.
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
	volatile uint32_t usart_clk = get_usart_clk();

	set_usart_pwr();

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
