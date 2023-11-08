/**
 * @file stm32_gpio_conf_f4.h
 * @brief 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-11
 */

#ifndef SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F4_H_
#define SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F4_H_

#include <stdint.h>

#include <bsp/stm32cube_hal.h>

#if 0
#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx)
#define STM32_GPIO_PORTS_COUNT 3 
#elif defined(STM32F411xE)
#define STM32_GPIO_PORTS_COUNT 5
#else
#define STM32_GPIO_PORTS_COUNT 8
#endif

static GPIO_TypeDef *stm32_gpio_ports[STM32_GPIO_PORTS_COUNT] = {
	GPIOA, GPIOB, GPIOC,
#if !defined(STM32F410Tx) && !defined(STM32F410Cx) && !defined(STM32F410Rx)
	GPIOD, GPIOE,
#if !defined(STM32F411xE)
    GPIOF, GPIOG,
    GPIOH
#endif
#endif /*!defined(STM32F410Tx) && !defined(STM32F410Cx) && !defined(STM32F410Rx)*/
};

static void stm32_gpio_clk_enable(void *gpio_base) {
	switch ((int) gpio_base) {
	case (int) GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (int) GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (int) GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#if !defined(STM32F410Tx) && !defined(STM32F410Cx) && !defined(STM32F410Rx)
	case (int) GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case (int) GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#if !defined(STM32F411xE)
	case (int) GPIOF: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
	case (int) GPIOG: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
	case (int) GPIOH: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif 
#endif /*!defined(STM32F410Tx) && !defined(STM32F410Cx) && !defined(STM32F410Rx) */
	}
}

#endif /* if 0 old version */

#endif /* SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F4_H_ */
