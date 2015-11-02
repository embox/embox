/**
 * @file uart.c
 * @brief Use UART interface to communicate between agents
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-30
 */

#include <stm32f3_discovery.h>
#include <stm32f3xx.h>
#include <string.h>

/**
 * @brief On STM32F3Discovery we are able to use up to 3 UART interfaces to
 * communicate with other agents. For each interface we have predefined ports
 * and pins:
 *     UART1: RX -> PC0
 *            TX -> PC1
 *
 *     UART2: RX -> PC2
 *            TX -> PC3
 *
 *     UART3: RX -> PA1
 *            TX -> PF2
 */
static GPIO_TypeDef *uart_ports_rx[] = { GPIOC,      GPIOC,      GPIOA};
static uint16_t      uart_pins_rx[]  = { GPIO_PIN_0, GPIO_PIN_2, GPIO_PIN_1};
static GPIO_TypeDef *uart_ports_tx[] = { GPIOC,      GPIOC,      GPIOF};
static uint16_t      uart_pins_tx[]  = { GPIO_PIN_1, GPIO_PIN_3, GPIO_PIN_2};

static void gpio_clk_enable(void *gpio) {
	switch ((int)gpio) {
	case (int)GPIOA:
		__GPIOA_CLK_ENABLE();
		break;
	case (int)GPIOB:
		__GPIOB_CLK_ENABLE();
		break;
	case (int)GPIOC:
		__GPIOC_CLK_ENABLE();
		break;
	case (int)GPIOD:
		__GPIOD_CLK_ENABLE();
		break;
	case (int)GPIOE:
		__GPIOE_CLK_ENABLE();
		break;
	case (int)GPIOF:
		__GPIOF_CLK_ENABLE();
		break;
	default:
		break;
	}
	return;
}

static void fill_gpio_init_rx(GPIO_InitTypeDef *a, int n) {
	memset(a, 0, sizeof(a));
	a->Pin       = uart_pins_rx[n];
	a->Mode      = GPIO_MODE_AF_OD;
	a->Pull      = GPIO_NOPULL;
	a->Speed     = GPIO_SPEED_HIGH;
}

static void fill_gpio_init_tx(GPIO_InitTypeDef *a, int n) {
	memset(a, 0, sizeof(a));
	a->Pin       = uart_pins_tx[n];
	a->Mode      = GPIO_MODE_AF_OD;
	a->Pull      = GPIO_NOPULL;
	a->Speed     = GPIO_SPEED_HIGH;
}

/**
 * @brief Initialize GPIO
 *
 * @param i_num
 */
static void gpio_config(int i_num) {
	GPIO_TypeDef *gpio_rx = uart_ports_rx[i_num];
	GPIO_TypeDef *gpio_tx = uart_ports_tx[i_num];
	GPIO_InitTypeDef gpio_init;

	gpio_clk_enable(gpio_rx);
	gpio_clk_enable(gpio_tx);

	fill_gpio_init_rx(&gpio_init, i_num);
	HAL_GPIO_Init(gpio_rx, &gpio_init);

	fill_gpio_init_tx(&gpio_init, i_num);
	HAL_GPIO_Init(gpio_tx, &gpio_init);
}

/**
 * @brief Initialize interface with given number
 *
 * @param i_num Number of UART interface
 *
 * @return Negative error number or 0 if succeed
 */
static void uart_init(int i_num) {
	USART_TypeDef * uart;

	switch (i_num) {
	case 0:
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		uart = USART1;
		__USART1_CLK_ENABLE();
		break;
	case 1:
		RCC->APB2ENR |= RCC_APB1ENR_USART2EN;
		uart = USART2;
		__USART2_CLK_ENABLE();
		break;
	case 2:
		RCC->APB2ENR |= RCC_APB1ENR_USART3EN;
		uart = USART3;
		__USART3_CLK_ENABLE();
		break;
	default:
		return;
	}

	uart->BRR  = 72000000 / 115200;
	uart->CR1 &= ~USART_CR1_M;
	uart->CR1 &= ~USART_CR1_PCE;
	uart->CR1 |= USART_CR1_TE;
	uart->CR1 |= USART_CR1_RE;
	uart->CR1 |= USART_CR1_UE;
	uart->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
}

int comm_init(void) {
	gpio_config(0);
	uart_init(0);

	return 0;
}
