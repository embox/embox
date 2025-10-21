/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

#include <drivers/spi.h>
#include <util/log.h>

#include "skeleton_spi.h"

static int skeleton_spi_setup(struct skeleton_spi *dev, bool is_master) {
	/* Set up your harware here */

	return 0;
}

int skeleton_spi_init(struct skeleton_spi *dev) {
	return skeleton_spi_setup(dev, true);
}

static int skeleton_spi_select(struct spi_controller *spi_dev, int cs) {
	log_debug("NIY");

	return 0;
}

static int skeleton_spi_set_mode(struct spi_controller *spi_dev, bool is_master) {
	struct skeleton_spi *dev = spi_dev->priv;
	return skeleton_spi_setup(dev, is_master);
}

static int skeleton_spi_transfer(struct spi_controller *spi_dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	//struct skeleton_spi *dev = spi_dev->priv;

	if (spi_dev->flags & SPI_CS_ACTIVE && spi_dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we lower the same pin all the tiem */
	}

	/* Place transmit/recieve code here */

	if (spi_dev->flags & SPI_CS_INACTIVE && spi_dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we raise the same pin all the tiem */
	}

	return 0;
}

struct spi_controller_ops skeleton_spic_ops = {
	.select   = skeleton_spi_select,
	.set_mode = skeleton_spi_set_mode,
	.transfer = skeleton_spi_transfer
};
