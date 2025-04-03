/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

#include <stdint.h>
#include <string.h>
#include <drivers/spi.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#include "pl022_spi.h"

#define SPI1_BASE_ADDR	  (uintptr_t)OPTION_GET(NUMBER,base_addr)

#define SPI_DEV_NAME      spi_bus_1

EMBOX_UNIT_INIT(pl022_spi1_init);

static struct pl022_spi pl022_spi1 = {
	.base_addr = SPI1_BASE_ADDR,

};

static int pl022_spi1_init(void) {
	pl022_spi_init(&pl022_spi1);

	return 0;
}

SPI_DEV_DEF(SPI_DEV_NAME, &pl022_spi_ops, &pl022_spi1, 1);
