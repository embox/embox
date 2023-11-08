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


#define STM32_GPIO_PORTS_COUNT \
	sizeof(stm32_gpio_ports) / sizeof(stm32_gpio_ports[0])

static GPIO_TypeDef *stm32_gpio_ports[] = {
	GPIOA,
	GPIOB,
	GPIOC,
#ifdef GPIOD
	GPIOD,
#ifdef GPIOE
	GPIOE,
#ifdef GPIOF
	GPIOF,
#ifdef GPIOG
	GPIOG,
#ifdef GPIOH
	GPIOH,
#ifdef GPIOI
	GPIOI,
#ifdef GPIOJ
	GPIOJ,
#ifdef GPIOK
	GPIOK
#endif /* GPIOK */
#endif /* GPIOJ */
#endif /* GPIOI */
#endif /* GPIOH */
#endif /* GPIOG */
#endif /* GPIOF */
#endif /* GPIOE */
#endif /* GPIOD */
};

static inline void stm32_gpio_clk_enable(void *gpio_base) {
	switch ((intptr_t) gpio_base) {
	case (intptr_t) GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
	case (intptr_t) GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
	case (intptr_t) GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#ifdef GPIOD
	case (intptr_t) GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#ifdef GPIOE
	case (intptr_t) GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#ifdef GPIOF
	case (intptr_t) GPIOF: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#ifdef GPIOG
	case (intptr_t) GPIOG: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
#ifdef GPIOH
	case (intptr_t) GPIOH: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#ifdef GPIOI
	case (intptr_t) GPIOI: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
#ifdef GPIOJ
	case (intptr_t) GPIOJ: __HAL_RCC_GPIOJ_CLK_ENABLE(); break;
#ifdef GPIOK
	case (intptr_t) GPIOK: __HAL_RCC_GPIOK_CLK_ENABLE(); break;
#endif /* GPIOK */
#endif /* GPIOJ */
#endif /* GPIOI */
#endif /* GPIOH */
#endif /* GPIOG */
#endif /* GPIOF */
#endif /* GPIOE */
#endif /* GPIOD */
	default: break;
	}
}


#endif /* SRC_DRIVERS_GPIO_STM32_USART_STM32_USART_CONF_F4_H_ */
