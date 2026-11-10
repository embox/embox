/**
 * @file
 * @brief
 *
 * @date 06.11.2026
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <framework/mod/options.h>
#include <util/log.h>
#include <drivers/spi.h>

#include "bmi160.h"
#include "bmi160_transfer.h"

#define BMI160_SPI_BUS   OPTION_GET(NUMBER, spi_bus)


struct bmi160_dev {
	int spi_bus;
	struct spi_device *spi_dev;
};

struct bmi160_dev bmi160_dev0 = {
	.spi_bus  = BMI160_SPI_BUS,
};

int bmi160_readb(struct bmi160_dev *dev, int offset, uint8_t *ret) {
	uint8_t in[2];
	uint8_t out[2];

	in[0] = offset | 0;
	in[1] = 0;
	if (spi_transfer(dev->spi_dev, in, out, 2) < 0) {
		log_debug("failed");
		return -1;
	}
	*ret = out[1];

	return 0;
}

int bmi160_writeb(struct bmi160_dev *dev,
	    int offset, uint8_t val) {
	uint8_t in[2];
	uint8_t out[2];

	in[0] = offset | 0;
	in[1] = val;
	if (spi_transfer(dev->spi_dev, in, out, 2) < 0) {
		log_debug("failed");
		return -1;
	}
	return 0;
}

int bmi160_hw_init(struct bmi160_dev *dev) {
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
