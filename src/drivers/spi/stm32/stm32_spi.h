/**
 * @file stm32_spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.12.2019
 */
#ifndef SRC_DRIVERS_SPI_STM32_I2C_H_
#define SRC_DRIVERS_SPI_STM32_I2C_H_

#include <stdint.h>

#if defined(STM32F407xx)
#include "stm32f4_discovery.h"
#elif defined (STM32F429xx)
#include "stm32f4xx_nucleo_144.h"
#elif defined (STM32L476xx)
#include "stm32l4xx_nucleo.h"
#elif defined (STM32L475xx)
#include "stm32l475e_iot01.h"
#else
#error Unsupported platform
#endif

struct stm32_spi {
	SPI_HandleTypeDef handle;
	GPIO_TypeDef *nss_port;
	uint32_t nss_pin;
};

extern int stm32_spi_init(struct stm32_spi *dev, void *instance);
extern struct spi_ops stm32_spi_ops;

#endif /* SRC_DRIVERS_SPI_STM32_I2C_H_ */
