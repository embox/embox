/**
 * @file
 *
 * @date Sep 25, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_F1_H_
#define SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_F1_H_

//#define STM32F1_CUBE_GPIO  1

#if 0
#if defined(STM32F101xE) || defined(STM32F103xE) || defined(STM32F101xG) || defined(STM32F103xG)
#define STM32_GPIO_PORTS_COUNT 6
#else
#define STM32_GPIO_PORTS_COUNT 5
#endif

static GPIO_TypeDef *stm32_gpio_ports[STM32_GPIO_PORTS_COUNT] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE,
#if defined(STM32F101xE) || defined(STM32F103xE) || defined(STM32F101xG) || defined(STM32F103xG)
	GPIOF
#endif
};

static void stm32_gpio_clk_enable(void *gpio_base) {
	switch ((int) gpio_base) {
	case (int) GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (int) GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (int) GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
	case (int) GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
	case (int) GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#if defined(STM32F101xE) || defined(STM32F103xE) || defined(STM32F101xG) || defined(STM32F103xG)
	case (int) GPIOF: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif
	}
}
#endif /*if 0 old vesion */


#endif /* SRC_DRIVERS_GPIO_STM32_STM32_GPIO_CONF_F1_H_ */
