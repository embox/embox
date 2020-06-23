/**
 * @file stm32_gpio_conf_l4.h
 * @brief
 * @author Puranjay Mohan <puranjay12@gmail.com@gmail.com>
 * @version 0.1
 * @date 24.06.2020
 */

#ifndef SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_L4_H_
#define SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_L4_H_

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gpio.h"

#define STM32_GPIO_PORTS_COUNT 8

static GPIO_TypeDef *stm32_gpio_ports[STM32_GPIO_PORTS_COUNT] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH
};

static void stm32_gpio_clk_enable(void *gpio_base) {
	switch ((int) gpio_base) {
	case (int) GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (int) GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (int) GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	case (int) GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case (int) GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
	case (int) GPIOF: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
	case (int) GPIOG: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
	case (int) GPIOH: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
	}
}

#endif /* SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_L4_H_ */
