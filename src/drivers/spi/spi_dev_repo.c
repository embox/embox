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
#include <util/log.h>

#include <assert.h>
#include <errno.h>

#include <drivers/spi.h>

#include <framework/mod/options.h>

ARRAY_SPREAD_DEF(struct spi_device *, __spi_device_registry);

/**
 * @brief Get SPI device pointer by it's ID
 */
struct spi_device *spi_dev_by_id(int id) {
	struct spi_device *dev;

	array_spread_foreach(dev, __spi_device_registry) {
		if(dev->spid_bus_num == id) {
			return dev;
		}
	}
	return NULL;
}

/**
 * @brief Return device ID by it's pointer
 */
int spi_dev_id(struct spi_device *spi_dev) {
	struct spi_device *dev;

	array_spread_foreach(dev, __spi_device_registry) {
		if(dev == spi_dev) {
			return dev->spid_bus_num;
		}
	}

	log_error("Wrong SPI device pointer: %p", spi_dev);
	return -1;
}
