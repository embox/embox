/**
 * @file
 * @brief
 *
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#include <assert.h>
#include <errno.h>

#include <drivers/spi.h>
#include <framework/mod/options.h>
#include <util/log.h>

ARRAY_SPREAD_DEF(struct spi_controller *, __spi_controller_registry);

/**
 * @brief Get SPI controller pointer by it's ID
 */
struct spi_controller *spi_controller_by_id(int id) {
	struct spi_controller *cntl;

	array_spread_foreach(cntl, __spi_controller_registry) {
		if (cntl->spic_bus_num == id) {
			return cntl;
		}
	}
	return NULL;
}

/**
 * @brief Return controller ID by it's pointer
 */
int spi_conrtoller_id(struct spi_controller *spi_ctrl) {
	struct spi_controller *cntl;

	array_spread_foreach(cntl, __spi_controller_registry) {
		if (cntl == spi_ctrl) {
			return cntl->spic_bus_num;
		}
	}

	log_error("Wrong SPI controller pointer: %p", spi_ctrl);
	return -1;
}
