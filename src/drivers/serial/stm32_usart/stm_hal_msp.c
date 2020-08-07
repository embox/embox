/**
 * @file
 * @brief
 * @data 20.11.2019
 * @author: Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <assert.h>

#include <drivers/serial/stm_usart.h>

extern void USART_RX_GPIO_CLK_ENABLE(void *usart_base);
extern void USART_TX_GPIO_CLK_ENABLE(void *usart_base);
extern void USART_CLK_ENABLE(void *usart_base);
extern uint16_t USART_RX_PIN(void *usart_base);
extern uint16_t USART_TX_PIN(void *usart_base);
extern uint8_t USART_RX_AF(void *usart_base);
extern uint8_t USART_TX_AF(void *usart_base);
extern GPIO_TypeDef *USART_RX_GPIO_PORT(void *usart_base);
extern GPIO_TypeDef *USART_TX_GPIO_PORT(void *usart_base);

void USART_CLK_ENABLE(void *usart_base) {
	if (usart_base == USART1) {
		__HAL_RCC_USART1_CLK_ENABLE();
	}
	else if (usart_base ==  USART2) {
		__HAL_RCC_USART2_CLK_ENABLE();
	}
#ifdef USART3
	else if (usart_base ==  USART3) {
		__HAL_RCC_USART3_CLK_ENABLE();
	}
#endif
#ifdef USART6
	else if (usart_base ==  USART6) {
		__HAL_RCC_USART6_CLK_ENABLE();
	}
#endif
	else {
		/* Not supported */
		assert(0);
	}
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	GPIO_InitTypeDef  GPIO_InitStruct;
	void *uart_base = huart->Instance;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	USART_TX_GPIO_CLK_ENABLE(uart_base);
	USART_RX_GPIO_CLK_ENABLE(uart_base);
	/* Enable USART2 clock */
	USART_CLK_ENABLE(uart_base);

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = USART_TX_PIN(uart_base);
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = USART_TX_AF(uart_base);

	HAL_GPIO_Init(USART_TX_GPIO_PORT(uart_base), &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = USART_RX_PIN(uart_base);
	GPIO_InitStruct.Alternate = USART_RX_AF(uart_base);

	HAL_GPIO_Init(USART_RX_GPIO_PORT(uart_base), &GPIO_InitStruct);
	/* TODO ##-3- Configure the NVIC for UART ########################################*/
}

