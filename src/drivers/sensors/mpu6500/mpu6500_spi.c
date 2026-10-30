/**
 * @file
 * @brief MPU6500 register reads over SPI.
 * @author Dmitrii Piliuk
 * @version
 * @date 28.10.2026
 */

#include <errno.h>
#include <stdint.h>

#include <drivers/spi.h>
#include <framework/mod/options.h>

#include "mpu6500_transfer.h"

#define MPU6500_SPI_READ 0x80

static struct spi_device *mpu6500_spi_dev;

int mpu6500_hw_init(void) {
	struct spi_device *dev;
	int ret;

	mpu6500_spi_dev = NULL;
	dev = spi_dev_by_id(OPTION_GET(NUMBER, spi_bus));
	if (!dev || !dev->spid_spi_cntl) {
		return -ENODEV;
	}

	dev->spid_flags = SPI_CS_MODE(SPI_MODE_0)
	    | SPI_CS_ACTIVE | SPI_CS_INACTIVE;
	ret = spi_set_master_mode(dev);
	if (ret) {
		return ret;
	}
	ret = spi_select(dev, OPTION_GET(NUMBER, spi_cs));
	if (ret) {
		return ret;
	}
	mpu6500_spi_dev = dev;
	return 0;
}

int mpu6500_readb(uint8_t reg, uint8_t *value) {
	uint8_t tx[2] = {reg | MPU6500_SPI_READ, 0};
	uint8_t rx[2];
	int ret;

	if (!value || reg > 0x7f) {
		return -EINVAL;
	}
	if (!mpu6500_spi_dev) {
		return -ENODEV;
	}

	/* Keep CS active across the register address and the returned byte. */
	ret = spi_transfer(mpu6500_spi_dev, tx, rx, sizeof(tx));
	if (ret) {
		return ret;
	}
	*value = rx[1];
	return 0;
}
