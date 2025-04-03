/**
 * @file skeleton_spi.h
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */
#ifndef SRC_DRIVERS_SPI_STM32_I2C_H_
#define SRC_DRIVERS_SPI_STM32_I2C_H_

#include <drivers/gpio.h>

struct skeleton_spi {
	void *instance;
	unsigned short nss_port;
	gpio_mask_t nss_pin;
	uint16_t clk_div;
	uint16_t bits_per_word;
};

extern int skeleton_spi_init(struct skeleton_spi *dev);
extern struct spi_ops skeleton_spi_ops;

#endif /* SRC_DRIVERS_SPI_STM32_I2C_H_ */
