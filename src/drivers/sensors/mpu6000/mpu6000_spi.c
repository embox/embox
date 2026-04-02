/**
 * @file
 * @brief
 *
 * @date 02.04.2026
 * @author Dmitry Pilyuk
 */
#include <stdint.h>

#include <drivers/spi.h>
#include <framework/mod/options.h>
#include <util/log.h>

#include "mpu6000_transfer.h"

#define MPU6000_SPI_BUS OPTION_GET(NUMBER, spi_bus)

#define MPU6000_SPI_READ  0x80
#define MPU6000_SPI_WRITE 0x00

struct mpu6000_dev {
	int spi_bus;
	struct spi_device *spi_dev;
};

struct mpu6000_dev mpu6000_dev0 = {
    .spi_bus = MPU6000_SPI_BUS,
};

int mpu6000_readb(struct mpu6000_dev *dev, int offset, uint8_t *ret) {
	uint8_t in[2];
	uint8_t out[2];

	in[0] = offset | MPU6000_SPI_READ;
	in[1] = 0;
	if (spi_transfer(dev->spi_dev, in, out, 2) < 0) {
		log_debug("failed");
		return -1;
	}
	*ret = out[1];

	return 0;
}

int mpu6000_writeb(struct mpu6000_dev *dev, int offset, uint8_t val) {
	uint8_t in[2];
	uint8_t out[2];

	in[0] = offset | MPU6000_SPI_WRITE;
	in[1] = val;
	if (spi_transfer(dev->spi_dev, in, out, 2) < 0) {
		log_debug("failed");
		return -1;
	}
	return 0;
}

int mpu6000_hw_init(struct mpu6000_dev *dev) {
	dev->spi_dev = spi_dev_by_id(dev->spi_bus);
	if (!dev->spi_dev) {
		return -1;
	}
	spi_set_master_mode(dev->spi_dev);

	spi_select(dev->spi_dev, 0);

	dev->spi_dev->spid_flags |= SPI_CS_ACTIVE;
	dev->spi_dev->spid_flags |= SPI_CS_INACTIVE;
	return 0;
}
