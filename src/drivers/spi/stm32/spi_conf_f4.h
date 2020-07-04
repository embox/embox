/**
 * @file spi_conf_f4.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.12.2019
 */

#ifndef SRC_DRIVERS_STM32_SPI_CONF_F4_H_
#define SRC_DRIVERS_STM32_SPI_CONF_F4_H_

#if defined(STM32F407xx)
#include "stm32f4_discovery.h"
#elif defined (STM32F429xx)
#include "stm32f4xx_nucleo_144.h"
#else
#error Unsupported platform
#endif
#include <drivers/gpio/gpio.h>

#if defined(STM32F407xx)
/* SPI1 */
#define SPI1_SCK_PIN               GPIO_PIN_3
#define SPI1_SCK_GPIO_PORT         GPIOB
#define SPI1_SCK_AF                GPIO_AF5_SPI1
#define SPI1_MISO_PIN              GPIO_PIN_4
#define SPI1_MISO_GPIO_PORT        GPIOB
#define SPI1_MISO_AF               GPIO_AF5_SPI1
#define SPI1_MOSI_PIN              GPIO_PIN_5
#define SPI1_MOSI_GPIO_PORT        GPIOB
#define SPI1_MOSI_AF               GPIO_AF5_SPI1
#define SPI1_NSS_GPIO_PORT         GPIOB
#define SPI1_NSS_PIN               GPIO_PIN_2

static inline void spi1_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

#elif defined (STM32F429xx)
/* SPI1 */
#define SPI1_SCK_PIN               GPIO_PIN_5
#define SPI1_SCK_GPIO_PORT         GPIOA
#define SPI1_SCK_AF                GPIO_AF5_SPI1
#define SPI1_MISO_PIN              GPIO_PIN_6
#define SPI1_MISO_GPIO_PORT        GPIOA
#define SPI1_MISO_AF               GPIO_AF5_SPI1
#define SPI1_MOSI_PIN              GPIO_PIN_5
#define SPI1_MOSI_GPIO_PORT        GPIOB
#define SPI1_MOSI_AF               GPIO_AF5_SPI1

#define SPI1_NSS_GPIO_PORT         GPIOA
#define SPI1_NSS_PIN               GPIO_PIN_4

static inline void spi1_enable_gpio_clocks(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

#endif

/* SPI2 */
#define SPI2_SCK_PIN               GPIO_PIN_13
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

static inline void spi1_enable_spi_clocks(void) {
	__HAL_RCC_SPI1_CLK_ENABLE();
}

static inline void spi2_enable_spi_clocks(void) {
	__HAL_RCC_SPI2_CLK_ENABLE();
}

#endif /* SRC_DRIVERS_STM32_SPI_CONF_F4_H_ */
