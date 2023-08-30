/**
 * @file
 *
 * @date Aug 14, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPIO_STM32CUBE_STM32_GPIO_CONF_F2_H_
#define SRC_DRIVERS_GPIO_STM32CUBE_STM32_GPIO_CONF_F2_H_

#include <stdint.h>

#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_gpio.h"

#define STM32_GPIO_PORTS_COUNT \
	sizeof(stm32_gpio_ports) / sizeof(stm32_gpio_ports[0])

static GPIO_TypeDef *stm32_gpio_ports[] = {
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
	GPIOG,
	GPIOH,
	GPIOI
};

static inline void stm32_gpio_clk_enable(void *gpio_base) {
	switch ((intptr_t) gpio_base) {
	case (intptr_t) GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (intptr_t) GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (intptr_t) GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	case (intptr_t) GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case (intptr_t) GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
	case (intptr_t) GPIOF: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
	case (intptr_t) GPIOG: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
	case (intptr_t) GPIOH: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
	case (intptr_t) GPIOI: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
	}
}

#endif /* SRC_DRIVERS_GPIO_STM32CUBE_STM32_GPIO_CONF_F2_H_ */
