/**
 * @file
 * @brief
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */
#include <stdint.h>
#include <framework/mod/options.h>
#include <util/log.h>
#include <drivers/spi.h>

#include "l3g4200d.h"
#include "l3g4200d_transfer.h"

#define L3G4200D_SPI_BUS   OPTION_GET(NUMBER, spi_bus)

#define L3G4200D_SPI_READ   0x80
#define L3G4200D_SPI_WRITE  0x00

struct l3g4200d_dev {
	int spi_bus;
	struct spi_device *spi_dev;
};

struct l3g4200d_dev l3g4200d_dev0 = {
	.spi_bus  = L3G4200D_SPI_BUS,
};

int l3g4200d_readb(struct l3g4200d_dev *dev, int offset, uint8_t *ret) {
	uint8_t in[2];
	uint8_t out[2];

	in[0] = offset | L3G4200D_SPI_READ;
	in[1] = 0;
	if (spi_transfer(dev->spi_dev, in, out, 2) < 0) {
		log_debug("failed");
		return -1;
	}
	*ret = out[1];

	return 0;
}

int l3g4200d_writeb(struct l3g4200d_dev *dev,
	    int offset, uint8_t val) {
	uint8_t in[2];
	uint8_t out[2];

	in[0] = offset | L3G4200D_SPI_WRITE;
	in[1] = val;
	if (spi_transfer(dev->spi_dev, in, out, 2) < 0) {
		log_debug("failed");
		return -1;
	}
	return 0;
}

int l3g4200d_hw_init(struct l3g4200d_dev *dev) {
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
