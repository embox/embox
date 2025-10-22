/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 29.03.2025
 */

#ifndef SRC_DRIVERS_SPI_PL022_PL022_SPI_H_
#define SRC_DRIVERS_SPI_PL022_PL022_SPI_H_

#include <stdint.h>

struct pl022_spi {
	struct spi_controller *spi_controller;
	uint32_t base_addr;
};

extern int pl022_spi_init(struct pl022_spi *dev);

extern struct spi_controller_ops pl022_spic_ops;

#endif /* SRC_DRIVERS_SPI_PL022_PL022_SPI_H_ */
