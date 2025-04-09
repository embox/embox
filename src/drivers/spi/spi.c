/**
 * @file spi.c
 * @brief NOTE: When you use spi.core without spi.devfs
 * you should make sure device-specific stuff is initialized
 * somehow else
 *
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#include <assert.h>
#include <errno.h>

#include <drivers/char_dev.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/log.h>

ARRAY_SPREAD_DECLARE(struct spi_device *, __spi_device_registry);
ARRAY_SPREAD_DECLARE(struct spi_controller *, __spi_controller_registry);

/**
 * @brief Call device-specific init handlers for all
 * registered SPI devices.
 *
 * @return Always 0
 */
static int spi_init(void) {
	struct spi_device *dev;
	struct spi_controller *ctrl;

	array_spread_foreach(ctrl, __spi_controller_registry) {
		assert(ctrl);

		if (!ctrl->spi_ops) {
			continue;
		}

		if (ctrl->spi_ops->init) {
			ctrl->spi_ops->init(ctrl);
		}
		else {
			log_warning("SPI%d has no init funcion", ctrl->spic_bus_num);
		}
	}

	array_spread_foreach(dev, __spi_device_registry) {
		assert(dev);
		dev->spid_spi_cntl = spi_controller_by_id(dev->spid_bus_num);
		if (!dev->spi_ops) {
			continue;
		}

		if (dev->spi_ops->init) {
			dev->spi_ops->init(dev);
		}
		else {
			log_warning("SPI%d has no init funcion", dev->spid_bus_num);
		}
	}

	return 0;
}
EMBOX_UNIT_INIT(spi_init);

/**
 * @brief Perform device-dependent SPI transfer operation
 *
 * @param dev SPI device
 * @param in  Data which is passed to controller
 * @param out Data which is received from controller
 * @param cnt Number of bytes to be passed
 *
 * @return Error code
 */
int spi_transfer(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt) {
	assert(dev);
	assert(dev->spi_ops);
	assert(in || out);

	if (dev->spi_ops->transfer == NULL) {
		log_debug("Transfer operation is not supported for SPI%d",
		    spi_dev_id(dev));
		return -ENOSUPP;
	}

	return dev->spi_ops->transfer(dev, in, out, cnt);
}

/**
 * @brief Perform device-spefic chip select operation
 */
int spi_select(struct spi_device *dev, int cs) {
	assert(dev);
	assert(dev->spi_ops);

	if (dev->spi_ops->select == NULL) {
		log_debug("Select operation is not supported for SPI%d",
		    spi_dev_id(dev));
		return -ENOSUPP;
	}

	return dev->spi_ops->select(dev, cs);
}

/**
 * @brief Set device to master or slave mode
 */
static int spi_set_mode(struct spi_device *dev, bool is_master) {
	assert(dev);
	assert(dev->spi_ops);

	if (dev->spi_ops->set_mode == NULL) {
		log_debug("SPI mode setting is not supported for SPI%d",
		    spi_dev_id(dev));
		return -ENOSUPP;
	}

	dev->is_master = is_master;

	return dev->spi_ops->set_mode(dev, is_master);
}

int spi_set_master_mode(struct spi_device *dev) {
	return spi_set_mode(dev, true);
}

int spi_set_slave_mode(struct spi_device *dev) {
	return spi_set_mode(dev, false);
}
