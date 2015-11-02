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

#include "communication.h"

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
static GPIO_TypeDef  *uart_ports_rx[] = { GPIOC,      GPIOA,      GPIOA};
static uint16_t       uart_pins_rx[]  = { GPIO_PIN_0, GPIO_PIN_2, GPIO_PIN_1};
static GPIO_TypeDef  *uart_ports_tx[] = { GPIOC,      GPIOA,      GPIOF};
static uint16_t       uart_pins_tx[]  = { GPIO_PIN_1, GPIO_PIN_3, GPIO_PIN_2};
static USART_TypeDef *uart_inums[]    = { USART1,     USART2,     USART3};

static uint8_t uart_tx_buf[3][UART_BUFF_SZ];
static uint8_t uart_rx_buf[3][UART_BUFF_SZ];

static USART_HandleTypeDef uart_handle[] = {
	{
		.Instance   = USART1,
		.pTxBuffPtr = &uart_tx_buf[0][0],
		.pRxBuffPtr = &uart_rx_buf[0][0],
		.State      = HAL_USART_STATE_RESET,
		.Init       = {
			.BaudRate   = UART_BAUD_RATE,
			.WordLength = USART_WORDLENGTH_8B,
			.Mode       = USART_MODE_TX_RX,
		},
	},
	{
		.Instance = USART2,
		.pTxBuffPtr = &uart_tx_buf[1][0],
		.pRxBuffPtr = &uart_rx_buf[1][0],
		.State = HAL_USART_STATE_RESET,
		.Init       = {
			.BaudRate   = UART_BAUD_RATE,
			.WordLength = USART_WORDLENGTH_8B,
			.Mode       = USART_MODE_TX_RX,
		},
	},
	{
		.Instance = USART3,
		.pTxBuffPtr = &uart_tx_buf[2][0],
		.pRxBuffPtr = &uart_rx_buf[2][0],
		.State = HAL_USART_STATE_RESET,
		.Init       = {
			.BaudRate   = UART_BAUD_RATE,
			.WordLength = USART_WORDLENGTH_8B,
			.Mode       = USART_MODE_TX_RX,
		},
	}
};

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
	switch (i_num) {
	case 0:
		__USART1_CLK_ENABLE();
		break;
	case 1:
		__USART2_CLK_ENABLE();
		break;
	case 2:
		__USART3_CLK_ENABLE();
		break;
	default:
		return;
	}

	HAL_USART_Init(&uart_handle[i_num]);
}

int comm_init(void) {
	gpio_config(1);
	uart_init(1);

	return 0;
}

int send_byte(uint8_t b, int i_num) {
	USART_TypeDef *uart = uart_inums[i_num];

	while (!(uart->ISR & USART_ISR_TXE));
	uart->TDR = (b & 0xFF);

	return 0;
}

uint8_t get_byte(int i_num) {
	USART_TypeDef *uart = uart_inums[i_num];
	while (!(uart->ISR & USART_ISR_RXNE));
	return (uint8_t)(uart->RDR & 0xFF);
}
