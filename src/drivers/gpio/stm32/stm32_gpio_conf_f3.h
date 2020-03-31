/**
 * @file stm32_gpio_conf_f4.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-11
 */

#ifndef SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F3_H_
#define SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F3_H_

#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

#define STM32_GPIO_PORTS_COUNT 6

static GPIO_TypeDef *stm32_gpio_ports[STM32_GPIO_PORTS_COUNT] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF
};

static void stm32_gpio_clk_enable(void *gpio_base) {
	switch ((int) gpio_base) {
	case (int) GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (int) GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (int) GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	case (int) GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case (int) GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
	case (int) GPIOF: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
	}
}

#endif /* SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F3_H_ */
