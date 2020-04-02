/**
 * @file
 *
 * @date 31.03.2020
 * @author Alexander Kalmuk
 */

#ifndef SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F7_H_
#define SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F7_H_

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_gpio.h"

#define STM32_GPIO_PORTS_COUNT 11

static GPIO_TypeDef *stm32_gpio_ports[STM32_GPIO_PORTS_COUNT] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH,
	GPIOI, GPIOJ, GPIOK
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
	case (int) GPIOI: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
	case (int) GPIOJ: __HAL_RCC_GPIOJ_CLK_ENABLE(); break;
	case (int) GPIOK: __HAL_RCC_GPIOK_CLK_ENABLE(); break;
	}
}

#endif /* SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F7_H_ */
