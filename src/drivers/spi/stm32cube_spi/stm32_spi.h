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

#include <bsp/stm32cube_hal.h>

/* For STM32F3 */
#ifndef GPIO_SPEED_FAST
# define GPIO_SPEED_FAST GPIO_SPEED_FREQ_HIGH
#endif

struct stm32_spi {
	int (*hw_init)(void);
	SPI_HandleTypeDef handle;
	unsigned short nss_port;
	uint32_t nss_pin;
	uint16_t clk_div;
	uint16_t bits_per_word;
};

extern int stm32_spi_init(struct stm32_spi *dev, void *instance);
extern struct spi_ops stm32_spi_ops;

#endif /* SRC_DRIVERS_SPI_STM32_I2C_H_ */
