/**
 * @file skeleton_spi.c
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 22.06.2023
 */

#include <drivers/spi.h>

//#include "skeleton_spi.h"

#include <drivers/gpio/gpio.h>
#include <config/board_config.h>
//#include <your-system-defines.h>
#define SPI0_BASE_ADDR	NULL
#define SPI1_BASE_ADDR	NULL

struct spi_ops skeleton_spi_ops;

struct skeleton_spi { // This is an example
	void *instance;
	unsigned short nss_port;
	gpio_mask_t nss_pin;
	uint16_t clk_div;
	uint16_t bits_per_word;
};

#define SKELETON_SPI_DEV_DEF(idx) \
	static struct skeleton_spi skeleton_spi##idx = {\
		.instance = SPI##idx##_BASE_ADDR,	\
	}

#ifdef CONF_SPI0_ENABLED
SKELETON_SPI_DEV_DEF(0);
SPI_DEV_DEF("spi0", &skeleton_spi_ops, &skeleton_spi0, 0);
#endif

#ifdef CONF_SPI1_ENABLED
SKELETON_SPI_DEV_DEF(1);
SPI_DEV_DEF("spi1", &skeleton_spi_ops, &skeleton_spi1, 1);
#endif

// It may be "skeleton_spi.h" up to this line

static int skeleton_spi_setup(struct skeleton_spi *dev, bool is_master) {

	/* Set up your hardware here */

	return 0;
}

static int skeleton_spi_init(struct spi_device *spi_dev) {
	struct skeleton_spi *dev = spi_dev->priv;

	/* Initialize your hardware here */

	skeleton_spi_setup(dev, true);

	return 0;
}

static int skeleton_spi_select(struct spi_device *spi_dev, int cs) {
	struct skeleton_spi *dev = spi_dev->priv;
	gpio_set(dev->nss_port, dev->nss_pin, cs);
	return 0;
}

static int skeleton_spi_set_mode(struct spi_device *spi_dev, bool is_master) {
	struct skeleton_spi *dev = spi_dev->priv;
	return skeleton_spi_setup(dev, is_master);
}

static int skeleton_spi_transfer(struct spi_device *spi_dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	struct skeleton_spi *dev = spi_dev->priv;

	if (spi_dev->flags & SPI_CS_ACTIVE && spi_dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we lower the same pin all the tiem */
		gpio_set(dev->nss_port, dev->nss_pin, GPIO_PIN_LOW);
	}

	/* Place transmit/recieve code here */

	if (spi_dev->flags & SPI_CS_INACTIVE && spi_dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we raise the same pin all the tiem */
		gpio_set(dev->nss_port, dev->nss_pin, GPIO_PIN_HIGH);
	}

	return 0;
}

struct spi_ops skeleton_spi_ops = {
	.init     = skeleton_spi_init,
	.select   = skeleton_spi_select,
	.set_mode = skeleton_spi_set_mode,
	.transfer = skeleton_spi_transfer
};
