/**
 * @file
 * @brief
 * @data 20.11.2019
 * @author: Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <assert.h>

#include <drivers/serial/stm_usart.h>

#include <config/board_config.h>

static void USART_CLK_ENABLE(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		CONF_USART1_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		CONF_USART2_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		CONF_USART3_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		CONF_USART4_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		CONF_USART5_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		CONF_USART6_CLK_ENABLE_UART();
		break;
	}
#endif
	default:
		assert(0);
	}
}

static void USART_TX_GPIO_CLK_ENABLE(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		CONF_USART1_CLK_ENABLE_TX();
		break;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		CONF_USART2_CLK_ENABLE_TX();
		break;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		CONF_USART3_CLK_ENABLE_TX();
		break;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		CONF_USART4_CLK_ENABLE_TX();
		break;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		CONF_USART5_CLK_ENABLE_TX();
		break;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		CONF_USART6_CLK_ENABLE_TX();
		break;
	}
#endif
	default:
		assert(0);
	}
}

static void USART_RX_GPIO_CLK_ENABLE(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		CONF_USART1_CLK_ENABLE_RX();
		break;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		CONF_USART2_CLK_ENABLE_RX();
		break;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		CONF_USART3_CLK_ENABLE_RX();
		break;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		CONF_USART4_CLK_ENABLE_RX();
		break;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		CONF_USART5_CLK_ENABLE_RX();
		break;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		CONF_USART6_CLK_ENABLE_RX();
		break;
	}
#endif
	default:
		assert(0);
	}
}

static uint16_t USART_RX_PIN(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		return CONF_USART1_PIN_RX_NR;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		return CONF_USART2_PIN_RX_NR;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		return CONF_USART3_PIN_RX_NR;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		return CONF_USART4_PIN_RX_NR;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		return CONF_USART5_PIN_RX_NR;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_RX_NR;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static uint16_t USART_TX_PIN(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		return CONF_USART1_PIN_TX_NR;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		return CONF_USART2_PIN_TX_NR;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		return CONF_USART3_PIN_TX_NR;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		return CONF_USART4_PIN_TX_NR;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		return CONF_USART5_PIN_TX_NR;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_TX_NR;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static GPIO_TypeDef *USART_RX_GPIO_PORT(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		return CONF_USART1_PIN_RX_PORT;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		return CONF_USART2_PIN_RX_PORT;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		return CONF_USART3_PIN_RX_PORT;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		return CONF_USART4_PIN_RX_PORT;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		return CONF_USART5_PIN_RX_PORT;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_RX_PORT;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static GPIO_TypeDef *USART_TX_GPIO_PORT(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		return CONF_USART1_PIN_TX_PORT;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		return CONF_USART2_PIN_TX_PORT;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		return CONF_USART3_PIN_TX_PORT;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		return CONF_USART4_PIN_TX_PORT;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		return CONF_USART5_PIN_TX_PORT;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_TX_PORT;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static uint8_t USART_TX_AF(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		return CONF_USART1_PIN_TX_AF;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		return CONF_USART2_PIN_TX_AF;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		return CONF_USART3_PIN_TX_AF;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		return CONF_USART4_PIN_TX_AF;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		return CONF_USART5_PIN_TX_AF;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_TX_AF;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static uint8_t USART_RX_AF(void *usart_base) {
	switch((uintptr_t)usart_base) {
#if defined(USART1) &&  defined(CONF_USART1_ENABLED)
	case (uintptr_t)USART1: {
		return CONF_USART1_PIN_RX_AF;
	}
#endif
#if defined(USART2) &&  defined(CONF_USART2_ENABLED)
	case (uintptr_t)USART2: {
		return CONF_USART2_PIN_RX_AF;
	}
#endif
#if defined(USART3) &&  defined(CONF_USART3_ENABLED)
	case (uintptr_t)USART3: {
		return CONF_USART3_PIN_RX_AF;
	}
#endif
#if defined(USART4) &&  defined(CONF_USART4_ENABLED)
	case (uintptr_t)USART4: {
		return CONF_USART4_PIN_RX_AF;
	}
#endif
#if defined(USART5) &&  defined(CONF_USART5_ENABLED)
	case (uintptr_t)USART5: {
		return CONF_USART5_PIN_RX_AF;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_RX_AF;
	}
#endif
	default:
		assert(0);
	}
	return 0;
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

