/**
 * @file spi_conf_f4.h
 * @brief
 * @author Puranjay Mohan <puranjay12@gmail.com>
 * @version
 * @date 23.06.2020
 */

#ifndef SRC_DRIVERS_STM32_SPI_CONF_L4_H_
#define SRC_DRIVERS_STM32_SPI_CONF_L4_H_

#if defined(STM32L475xx)
#include "stm32l475e_iot01.h"
#elif defined (STM32L476xx)
#include "stm32l4xx_nucleo.h"
#else
#error Unsupported platform
#endif
#include <drivers/gpio/gpio.h>

#if defined(STM32L476xx)
/* SPI1 */
#define SPI1_SCK_PIN               GPIO_PIN_5
#define SPI1_SCK_GPIO_PORT         GPIOA
#define SPI1_SCK_AF                GPIO_AF5_SPI1
#define SPI1_MISO_PIN              GPIO_PIN_6
#define SPI1_MISO_GPIO_PORT        GPIOA
#define SPI1_MISO_AF               GPIO_AF5_SPI1
#define SPI1_MOSI_PIN              GPIO_PIN_7
#define SPI1_MOSI_GPIO_PORT        GPIOA
#define SPI1_MOSI_AF               GPIO_AF5_SPI1
#define SPI1_NSS_GPIO_PORT         GPIOA
#define SPI1_NSS_PIN               GPIO_PIN_4

static inline void spi1_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
}

#elif defined (STM32L475xx)
/* SPI1 */
#define SPI1_SCK_PIN               GPIO_PIN_5
#define SPI1_SCK_GPIO_PORT         GPIOA
#define SPI1_SCK_AF                GPIO_AF5_SPI1
#define SPI1_MISO_PIN              GPIO_PIN_6
#define SPI1_MISO_GPIO_PORT        GPIOA
#define SPI1_MISO_AF               GPIO_AF5_SPI1
#define SPI1_MOSI_PIN              GPIO_PIN_7
#define SPI1_MOSI_GPIO_PORT        GPIOA
#define SPI1_MOSI_AF               GPIO_AF5_SPI1

#define SPI1_NSS_GPIO_PORT         GPIOA
#define SPI1_NSS_PIN               GPIO_PIN_15

static inline void spi1_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
}

#endif

#if defined(STM32L476xx)
/* SPI2 */
#define SPI2_SCK_PIN               GPIO_PIN_10
#define SPI2_SCK_GPIO_PORT         GPIOB
#define SPI2_SCK_AF                GPIO_AF5_SPI2
#define SPI2_MISO_PIN              GPIO_PIN_14
#define SPI2_MISO_GPIO_PORT        GPIOB
#define SPI2_MISO_AF               GPIO_AF5_SPI2
#define SPI2_MOSI_PIN              GPIO_PIN_15
#define SPI2_MOSI_GPIO_PORT        GPIOB
#define SPI2_MOSI_AF               GPIO_AF5_SPI2
#define SPI2_NSS_GPIO_PORT         GPIOB
#define SPI2_NSS_PIN               GPIO_PIN_12

static inline void spi2_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

#endif

static inline void spi1_enable_spi_clocks(void) {
	__HAL_RCC_SPI1_CLK_ENABLE();
}

static inline void spi2_enable_spi_clocks(void) {
	__HAL_RCC_SPI2_CLK_ENABLE();
}

#endif /* SRC_DRIVERS_STM32_SPI_CONF_L4_H_ */
