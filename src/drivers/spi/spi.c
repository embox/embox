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

#include <drivers/spi.h>
#include <util/log.h>

#include <assert.h>
#include <errno.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

/* Assume we have 4 SPI devices at most */
#if SPI_REGISTRY_SZ >0
struct spi_device spi_device0 __attribute__((weak));
#elif SPI_REGISTRY_SZ >1
struct spi_device spi_device1 __attribute__((weak));
#elif SPI_REGISTRY_SZ >2
struct spi_device spi_device2 __attribute__((weak));
#elif SPI_REGISTRY_SZ >3
struct spi_device spi_device3 __attribute__((weak));
#elif SPI_REGISTRY_SZ >4
struct spi_device spi_device4 __attribute__((weak));
#elif SPI_REGISTRY_SZ >5
struct spi_device spi_device5 __attribute__((weak));
#endif

/* Note: it's array of pointers, not structures as usual */
struct spi_device *spi_device_registry[] = {
	#if SPI_REGISTRY_SZ >0
	&spi_device0,
	#elif SPI_REGISTRY_SZ >1
	&spi_device1,
	#elif SPI_REGISTRY_SZ >2
	&spi_device2,
	#elif SPI_REGISTRY_SZ >3
	&spi_device3,
	#elif SPI_REGISTRY_SZ >4
	&spi_device4,
	#elif SPI_REGISTRY_SZ >5
	&spi_device5
	#endif
};

/**
 * @brief Call device-specific init handlers for all
 * registered SPI devices.
 *
 * @return Always 0
 */
static int spi_init(void) {
	struct spi_device *dev;


	for (int i = 0; i < SPI_REGISTRY_SZ; i++) {
		dev = spi_device_registry[i];

		assert(dev);

		if (!dev->spi_ops) {
			continue;
		}

		if (dev->spi_ops->init) {
			dev->spi_ops->init(dev);
		} else {
			log_warning("SPI%d has no init funcion", i);
		}
	}

	return 0;
}
EMBOX_UNIT_INIT(spi_init);

/**
 * @brief Get SPI device pointer by it's ID
 */
struct spi_device *spi_dev_by_id(int id) {
	if (id < 0 || id >= SPI_REGISTRY_SZ) {
		return NULL;
	}

	if (spi_device_registry[id]->spi_ops != NULL) {
		return spi_device_registry[id];
	} else {
		return NULL;
	}
}

/**
 * @brief Return device ID by it's pointer
 */
int spi_dev_id(struct spi_device *dev) {
	for (int i = 0; i < SPI_REGISTRY_SZ; i++) {
		if (dev == spi_device_registry[i] &&
				spi_device_registry[i]->spi_ops != NULL) {
			return i;
		}
	}

	log_error("Wrong SPI device pointer: %p", dev);
	return -1;
}

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

/* Stub */
const struct idesc_ops spi_iops __attribute__ ((weak));
