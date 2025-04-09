/**
 * @file
 * @brief
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

ARRAY_SPREAD_DEF(struct spi_controller *, __spi_controller_registry);

/**
 * @brief Get SPI controller pointer by it's ID
 */
struct spi_controller *spi_controller_by_id(int id) {
	struct spi_controller *dev;

	array_spread_foreach(dev, __spi_controller_registry) {
		if(dev->spic_bus_num == id) {
			return dev;
		}
	}
	return NULL;
}

/**
 * @brief Return controller ID by it's pointer
 */
int spi_conrtoller_id(struct spi_controller *spi_ctrl) {
	struct spi_controller *dev;

	array_spread_foreach(dev, __spi_controller_registry) {
		if(dev == spi_ctrl) {
			return dev->spic_bus_num;
		}
	}

	log_error("Wrong SPI controller pointer: %p", spi_ctrl);
	return -1;
}
