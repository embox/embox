/**
 * @file
 * @brief i.MX6 Enhanced Configurable SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 12.06.2017
 */

#ifndef SRC_DRIVERS_SPI_IMX6_ECSPI_H_
#define SRC_DRIVERS_SPI_IMX6_ECSPI_H_

#include <stdint.h>
#include <drivers/gpio/gpio.h>

struct imx6_ecspi {
	uint32_t base_addr;
	int cs;
};

extern int imx6_ecspi_init(struct imx6_ecspi *dev);
extern struct spi_ops imx6_ecspi_ops;

#endif /* SRC_DRIVERS_SPI_IMX6_ECSPI_H_ */
