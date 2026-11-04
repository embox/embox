/**
 * @file
 * @brief BMP280 four-wire SPI transport.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <drivers/spi.h>
#include <framework/mod/options.h>

#include "bmp280_transfer.h"

#define BMP280_SPI_MAX_READ 24

struct bmp280_dev {
	struct spi_device *spi;
};

struct bmp280_dev bmp280_dev0;

int bmp280_hw_init(struct bmp280_dev *dev) {
	struct spi_device *spi;
	int ret;

	dev->spi = NULL;
	spi = spi_dev_by_id(OPTION_GET(NUMBER, spi_bus));
	if (!spi || !spi->spid_spi_cntl) {
		return -ENODEV;
	}
	spi->spid_flags = SPI_CS_MODE(SPI_MODE_0) | SPI_CS_ACTIVE | SPI_CS_INACTIVE;
	ret = spi_set_master_mode(spi);
	if (ret) {
		return ret;
	}
	ret = spi_select(spi, OPTION_GET(NUMBER, spi_cs));
	if (ret) {
		return ret;
	}
	dev->spi = spi;
	return 0;
}

int bmp280_read(struct bmp280_dev *dev, int offset, uint8_t *data, size_t len) {
	uint8_t tx[BMP280_SPI_MAX_READ + 1] = {0};
	uint8_t rx[BMP280_SPI_MAX_READ + 1];
	int ret;

	if (offset < 0x80 || offset > 0xff || !data || !len) {
		return -EINVAL;
	}
	if (len > BMP280_SPI_MAX_READ || len > 0x100U - offset) {
		return -EMSGSIZE;
	}
	if (!dev->spi) {
		return -ENODEV;
	}
	/* Address plus dummy clocks in one transfer keeps CS asserted. */
	tx[0] = (offset & 0x7f) | 0x80;
	ret = spi_transfer(dev->spi, tx, rx, len + 1);
	if (ret) {
		return ret;
	}
	memcpy(data, rx + 1, len);
	return 0;
}

int bmp280_readb(struct bmp280_dev *dev, int offset, uint8_t *data) {
	return bmp280_read(dev, offset, data, 1);
}

int bmp280_writeb(struct bmp280_dev *dev, int offset, uint8_t value) {
	uint8_t tx[] = {offset & 0x7f, value};

	if (offset < 0x80 || offset > 0xff) {
		return -EINVAL;
	}
	if (!dev->spi) {
		return -ENODEV;
	}
	return spi_transfer(dev->spi, tx, NULL, sizeof(tx));
}
