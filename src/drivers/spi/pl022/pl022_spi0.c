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

#define SPI_BUS_NUM        0

#define SPI_BASE_ADDR	  (uintptr_t)OPTION_GET(NUMBER,base_addr)

#define SPI_BUS_NAME      MACRO_CONCAT(spibus,SPI_BUS_NUM)

EMBOX_UNIT_INIT(pl022_spi0_init);

static struct pl022_spi pl022_spi0 = {
	.base_addr = SPI_BASE_ADDR,
};

static int pl022_spi0_init(void) {
	pl022_spi_init(&pl022_spi0);

	return 0;
}

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &pl022_spi_ops, &pl022_spi0, SPI_BUS_NUM);
