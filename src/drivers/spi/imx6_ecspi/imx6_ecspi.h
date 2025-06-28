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
#include <drivers/gpio.h>

#define IMX6_ECSPI_MAX_CS   4

struct imx6_ecspi {
	uint32_t base_addr;
	int cs; /* current CS */
	unsigned int cs_count; /* size of cs_array */
	/* Format is [gpio][port], more
	 * details in IMX6DQRM.pdf at page 426 */
	unsigned char cs_array[IMX6_ECSPI_MAX_CS][2];
	int cs_iomux[IMX6_ECSPI_MAX_CS];
};

extern int imx6_ecspi_init(struct imx6_ecspi *dev);
extern struct spi_controller_ops imx6_ecspi_ops;

#endif /* SRC_DRIVERS_SPI_IMX6_ECSPI_H_ */
