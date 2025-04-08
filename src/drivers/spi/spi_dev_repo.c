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
#if 0
/* Assume we have 4 SPI devices at most */
#if SPI_REGISTRY_SZ > 0
struct spi_device spi_device0 __attribute__((weak));
#endif
#if SPI_REGISTRY_SZ > 1
struct spi_device spi_device1 __attribute__((weak));
#endif
#if SPI_REGISTRY_SZ > 2
struct spi_device spi_device2 __attribute__((weak));
#endif
#if SPI_REGISTRY_SZ > 3
struct spi_device spi_device3 __attribute__((weak));
#endif
#if SPI_REGISTRY_SZ > 4
struct spi_device spi_device4 __attribute__((weak));
#endif
#if SPI_REGISTRY_SZ > 5
struct spi_device spi_device5 __attribute__((weak));
#endif

/* Note: it's array of pointers, not structures as usual */
struct spi_device *spi_device_registry[SPI_REGISTRY_SZ] = {
#if SPI_REGISTRY_SZ > 0
    &spi_device0,
#endif
#if SPI_REGISTRY_SZ > 1
    &spi_device1,
#endif
#if SPI_REGISTRY_SZ > 1
    &spi_device2,
#endif
#if SPI_REGISTRY_SZ > 1
    &spi_device3,
#endif
#if SPI_REGISTRY_SZ > 1
    &spi_device4,
#endif
#if SPI_REGISTRY_SZ > 1
    &spi_device5
#endif
};
#endif
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
