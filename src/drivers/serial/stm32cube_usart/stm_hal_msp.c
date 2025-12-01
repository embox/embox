/**
 * @file
 * @brief
 * @date 20.11.2019
 * @author: Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <assert.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include <drivers/gpio.h>

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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		CONF_UART4_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		CONF_UART5_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		CONF_USART6_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		CONF_UART7_CLK_ENABLE_UART();
		break;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		CONF_UART8_CLK_ENABLE_UART();
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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		return CONF_UART4_PIN_RX_NR;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		return CONF_UART5_PIN_RX_NR;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_RX_NR;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		return CONF_UART7_PIN_RX_NR;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		return CONF_UART8_PIN_RX_NR;
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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		return CONF_UART4_PIN_TX_NR;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		return CONF_UART5_PIN_TX_NR;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_TX_NR;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		return CONF_UART7_PIN_TX_NR;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		return CONF_UART8_PIN_TX_NR;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static int USART_RX_GPIO_PORT(void *usart_base) {
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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		return CONF_UART4_PIN_RX_PORT;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		return CONF_UART5_PIN_RX_PORT;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_RX_PORT;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		return CONF_UART7_PIN_RX_PORT;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		return CONF_UART8_PIN_RX_PORT;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

static int USART_TX_GPIO_PORT(void *usart_base) {
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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		return CONF_UART4_PIN_TX_PORT;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		return CONF_UART5_PIN_TX_PORT;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_TX_PORT;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		return CONF_UART7_PIN_TX_PORT;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		return CONF_UART8_PIN_TX_PORT;
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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		return CONF_UART4_PIN_TX_AF;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		return CONF_UART5_PIN_TX_AF;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_TX_AF;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		return CONF_UART7_PIN_TX_AF;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		return CONF_UART8_PIN_TX_AF;
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
#if defined(UART4) &&  defined(CONF_UART4_ENABLED)
	case (uintptr_t)UART4: {
		return CONF_UART4_PIN_RX_AF;
	}
#endif
#if defined(UART5) &&  defined(CONF_UART5_ENABLED)
	case (uintptr_t)UART5: {
		return CONF_UART5_PIN_RX_AF;
	}
#endif
#if defined(USART6) &&  defined(CONF_USART6_ENABLED)
	case (uintptr_t)USART6: {
		return CONF_USART6_PIN_RX_AF;
	}
#endif
#if defined(UART7) &&  defined(CONF_UART7_ENABLED)
	case (uintptr_t)UART7: {
		return CONF_UART7_PIN_RX_AF;
	}
#endif
#if defined(UART8) &&  defined(CONF_UART8_ENABLED)
	case (uintptr_t)UART8: {
		return CONF_UART8_PIN_RX_AF;
	}
#endif
	default:
		assert(0);
	}
	return 0;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	void *uart_base = huart->Instance;

	/* Enable USART2 clock */
	USART_CLK_ENABLE(uart_base);

	gpio_setup_mode(USART_TX_GPIO_PORT(uart_base),
				(1 << USART_TX_PIN(uart_base)),
				/* GPIO_MODE_OUT | */ 
				GPIO_MODE_ALT_SET(USART_TX_AF(uart_base))
						| GPIO_MODE_OUT_PUSH_PULL);

	gpio_setup_mode(USART_RX_GPIO_PORT(uart_base),
				(1 << USART_RX_PIN(uart_base)),
				/* GPIO_MODE_IN |*/
				GPIO_MODE_ALT_SET(USART_RX_AF(uart_base))
						| GPIO_MODE_OUT_PUSH_PULL);

}

